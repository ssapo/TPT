#include "TPTVirtualJoystick.h"
#include "TPTPC_Impl.h"

#include "SlateBlueprintLibrary.h"

#include "WidgetLayoutLibrary.h"

#include "Engine/UserInterfaceSettings.h"
#include "SlateApplication.h"
#include "SlateBlueprintLibrary.h"
#include "Rendering/DrawElements.h"
#include "GenericApplicationMessageHandler.h"

const float OPACITY_LERP_RATE = 3.0f;

static FORCEINLINE float GetScaleFactor(const FGeometry& Geometry)
{
	const FVector2D& LocalSize = Geometry.GetLocalSize();
	FIntPoint Size((int32)LocalSize.X, (int32)LocalSize.Y);
	return GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(Size);
}

static int32 ResolveRelativePosition(float Position, float RelativeTo, float ScaleFactor)
{
	// absolute from edge
	if (Position < -1.0f)
	{
		return RelativeTo + Position * ScaleFactor;
	}
	// relative from edge
	else if (Position < 0.0f)
	{
		return RelativeTo + Position * RelativeTo;
	}
	// relative from 0
	else if (Position <= 1.0f)
	{
		return Position * RelativeTo;
	}
	// absolute from 0
	else
	{
		return Position * ScaleFactor;
	}
}

void STPTVirtualJoystick::FTPTControlInfo::Reset()
{
	// snap the visual center back to normal (for controls that have a center on touch)
	VisualCenter = CorrectedCenter;
}

void STPTVirtualJoystick::Construct(const FArguments& InArgs)
{
	// ---------------------------------- Left Controller ------------------------------------------
	bTouchStarted = false;
	bFirstTick = true;

	LastThumbDelta = FVector2D::ZeroVector;
	ThumbLocation = FVector2D::ZeroVector;
	FingerIndex = -1;
	ThumbMaxRadius = 50.0f;

	// -------------------------------------- Arguments --------------------------------------------
	Owner = InArgs._Owner;
	OnThumbMovementStarted = InArgs._OnMovementStart;
	OnThumbMovementDeltaFromCenter = InArgs._OnThumbDeltaEvent;
	OnThumbMovementIsOver = InArgs._OnMovementOver;

	// --------------------------- VirtualJoystick Construct ---------------------------------------
	State = State_Inactive;
	bVisible = true;
	bPreventReCenter = false;

	// just set some defaults
	ActiveOpacity = 1.0f;
	InactiveOpacity = 0.1f;
	TimeUntilDeactive = 0.5f;
	TimeUntilReset = 2.0f;
	ActivationDelay = 0.f;
	CurrentOpacity = InactiveOpacity;
	StartupDelay = 0.f;

	// listen for displaymetrics changes to reposition controls
	FSlateApplication::Get().GetPlatformApplication()->OnDisplayMetricsChanged().AddSP(this, &STPTVirtualJoystick::HandleDisplayMetricsChanged);
}

FReply STPTVirtualJoystick::OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	if (bVisible == false)
	{
		return FReply::Unhandled();
	}

	if (Owner.IsValid() == false)
	{
		return FReply::Unhandled();
	}

	FReply ReturnValue = FReply::Unhandled();
	if (FingerIndex == -1)
	{
		ReturnValue = Super_OnTouchStarted(MyGeometry, Event);
		if (ReturnValue.IsEventHandled())
		{
			bTouchStarted = true;
			bFirstTick = true;

			FVector2D PixelPosition, ViewportPosition;
			USlateBlueprintLibrary::AbsoluteToViewport(Owner.Get(), Event.GetScreenSpacePosition(),
				PixelPosition,
				ViewportPosition);

			CursorLocation = ViewportPosition;
			NewTouchCenterLocation = CursorLocation;

			FingerIndex = Event.GetPointerIndex();
			OnThumbMovementStarted.ExecuteIfBound();
		}
	}

	return ReturnValue;
}

FReply STPTVirtualJoystick::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	if (bVisible == false)
	{
		return FReply::Unhandled();
	}

	if (Owner.IsValid() == false)
	{
		return FReply::Unhandled();
	}

	FReply ReturnValue = Super_OnTouchMoved(MyGeometry, Event);

	if (FingerIndex == Event.GetPointerIndex())
	{
		FVector2D PixelPosition, ViewportPosition;
		USlateBlueprintLibrary::AbsoluteToViewport(Owner.Get(), Event.GetScreenSpacePosition(),
			PixelPosition,
			ViewportPosition);

		CursorLocation = ViewportPosition;
	}

	return ReturnValue;
}

FReply STPTVirtualJoystick::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	if (bVisible == false)
	{
		return FReply::Unhandled();
	}

	if (Owner.IsValid() == false)
	{
		return FReply::Unhandled();
	}

	if (FingerIndex == Event.GetPointerIndex())
	{
		FingerIndex = -1;

		bTouchStarted = false;
		bFirstTick = false;

		LastThumbDelta = FVector2D::ZeroVector;
		ThumbLocation = FVector2D::ZeroVector;

		OnThumbMovementIsOver.ExecuteIfBound();
	}

	return Super_OnTouchEnded(MyGeometry, Event);
}

void STPTVirtualJoystick::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (bVisible == false)
	{
		return;
	}

	if (Owner.IsValid() == false)
	{
		return;
	}

	Super_Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (bTouchStarted)
	{
		if (bFirstTick)
		{
			bFirstTick = false;
			return;
		}

		CalculateThumLocAndDeltaFromNewCenter(AllottedGeometry);
		UpdateThumbLocationAndDelta();

		OnThumbMovementDeltaFromCenter.ExecuteIfBound(ThumbLocation, LastThumbDelta);
	}
}

static bool PositionIsInside(const FVector2D& Center, const FVector2D& Position, const FVector2D& BoxSize)
{
	return
		Position.X >= Center.X - BoxSize.X * 0.5f &&
		Position.X <= Center.X + BoxSize.X * 0.5f &&
		Position.Y >= Center.Y - BoxSize.Y * 0.5f &&
		Position.Y <= Center.Y + BoxSize.Y * 0.5f;
}


int32 STPTVirtualJoystick::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 RetLayerId = LayerId;

	if (bVisible)
	{
		FLinearColor ColorAndOpacitySRGB = InWidgetStyle.GetColorAndOpacityTint();
		ColorAndOpacitySRGB.A = CurrentOpacity;

		for (int32 ControlIndex = 0; ControlIndex < MyControls.Num(); ControlIndex++)
		{
			const FTPTControlInfo& Control = MyControls[ControlIndex];

			if (Control.Image2.IsValid())
			{
				FSlateDrawElement::MakeBox(
					OutDrawElements,
					RetLayerId++,
					AllottedGeometry.ToPaintGeometry(
						Control.VisualCenter - FVector2D(Control.CorrectedVisualSize.X * 0.5f, Control.CorrectedVisualSize.Y * 0.5f),
						Control.CorrectedVisualSize),
					Control.Image2->GetSlateBrush(),
					ESlateDrawEffect::None,
					ColorAndOpacitySRGB
				);
			}

			if (Control.Image1.IsValid())
			{
				FSlateDrawElement::MakeBox(
					OutDrawElements,
					RetLayerId++,
					AllottedGeometry.ToPaintGeometry(
						Control.VisualCenter + Control.ThumbPosition - FVector2D(Control.CorrectedThumbSize.X * 0.5f, Control.CorrectedThumbSize.Y * 0.5f),
						Control.CorrectedThumbSize),
					Control.Image1->GetSlateBrush(),
					ESlateDrawEffect::None,
					ColorAndOpacitySRGB
				);
			}
		}
	}

	return RetLayerId;
}

void STPTVirtualJoystick::CopyControls()
{
	for (const FControlInfo& Each : Controls)
	{
		FTPTControlInfo NewControl;
		NewControl.Image1 = Each.Image1;
		NewControl.Image2 = Each.Image2;
		NewControl.Center = Each.Center;
		NewControl.VisualSize = Each.VisualSize;
		NewControl.ThumbSize = Each.ThumbSize;
		NewControl.InteractionSize = Each.InteractionSize;
		NewControl.InputScale = Each.InputScale;
		NewControl.MainInputKey = Each.MainInputKey;
		NewControl.AltInputKey = Each.AltInputKey;
		NewControl.PositionedCenter = Each.PositionedCenter;

		MyControls.Add(NewControl);
	}
}

void STPTVirtualJoystick::CalculateThumLocAndDeltaFromNewCenter(const FGeometry& MyGeometry)
{
	LastThumbDelta = CursorLocation - NewTouchCenterLocation;

	FVector2D PixelPosition;
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::LocalToViewport(Owner.Get(), MyGeometry, FVector2D::ZeroVector,
		PixelPosition,
		ViewportPosition);

	ThumbLocation = ViewportPosition;
}

void STPTVirtualJoystick::UpdateThumbLocationAndDelta()
{
	FVector2D DeltaNorm = LastThumbDelta.GetSafeNormal();

	float DeltaLength = LastThumbDelta.Size();
	DeltaNorm *= FMath::Clamp(DeltaLength, 1.0f, ThumbMaxRadius);

	ThumbLocation = NewTouchCenterLocation + DeltaNorm;
}


FReply STPTVirtualJoystick::Super_OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	//	UE_LOG(LogTemp, Log, TEXT("Pointer index: %d"), Event.GetPointerIndex());

	FVector2D LocalCoord = MyGeometry.AbsoluteToLocal(Event.GetScreenSpacePosition());

	for (int32 ControlIndex = 0; ControlIndex < MyControls.Num(); ControlIndex++)
	{
		FTPTControlInfo& Control = MyControls[ControlIndex];

		// skip controls already in use
		if (Control.CapturedPointerIndex == -1)
		{
			if (PositionIsInside(Control.CorrectedCenter, LocalCoord, Control.CorrectedInteractionSize))
			{

				// Align Joystick inside of Screen
				//AlignBoxIntoScreen(LocalCoord, Control.CorrectedVisualSize, MyGeometry.GetLocalSize());

				Control.CapturedPointerIndex = Event.GetPointerIndex();

				if (ActivationDelay == 0.f)
				{
					CurrentOpacity = ActiveOpacity;

					if (!bPreventReCenter)
					{
						Control.VisualCenter = LocalCoord;
					}

					if (HandleTouch(ControlIndex, LocalCoord, MyGeometry.GetLocalSize())) // Never fail!
					{
						return FReply::Handled().CaptureMouse(SharedThis(this));
					}
				}
				else
				{
					Control.bNeedUpdatedCenter = true;
					Control.ElapsedTime = 0.f;
					Control.NextCenter = LocalCoord;

					return FReply::Unhandled();
				}
			}
		}
	}

	//	CapturedPointerIndex[CapturedJoystick] = -1;

	return FReply::Unhandled();
}

FReply STPTVirtualJoystick::Super_OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	FVector2D LocalCoord = MyGeometry.AbsoluteToLocal(Event.GetScreenSpacePosition());

	for (int32 ControlIndex = 0; ControlIndex < MyControls.Num(); ControlIndex++)
	{
		FTPTControlInfo& Control = MyControls[ControlIndex];

		// is this control the one captured to this pointer?
		if (Control.CapturedPointerIndex == Event.GetPointerIndex())
		{
			if (Control.bNeedUpdatedCenter)
			{
				return FReply::Unhandled();
			}
			else if (HandleTouch(ControlIndex, LocalCoord, MyGeometry.GetLocalSize()))
			{
				return FReply::Handled();
			}
		}
	}

	return FReply::Unhandled();
}

FReply STPTVirtualJoystick::Super_OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	for (int32 ControlIndex = 0; ControlIndex < MyControls.Num(); ControlIndex++)
	{
		FTPTControlInfo& Control = MyControls[ControlIndex];

		// is this control the one captured to this pointer?
		if (Control.CapturedPointerIndex == Event.GetPointerIndex())
		{
			// release and center the joystick
			Control.ThumbPosition = FVector2D(0, 0);
			Control.CapturedPointerIndex = -1;

			// send one more joystick update for the centering
			Control.bSendOneMoreEvent = true;

			// Pass event as unhandled if time is too short
			if (Control.bNeedUpdatedCenter)
			{
				Control.bNeedUpdatedCenter = false;
				return FReply::Unhandled();
			}

			return FReply::Handled().ReleaseMouseCapture();
		}
	}

	return FReply::Unhandled();
}

void STPTVirtualJoystick::Super_Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (State == State_WaitForStart || State == State_CountingDownToStart)
	{
		CurrentOpacity = 0.f;
	}
	else
	{
		// lerp to the desired opacity based on whether the user is interacting with the joystick
		CurrentOpacity = FMath::Lerp(CurrentOpacity, GetBaseOpacity(), OPACITY_LERP_RATE * InDeltaTime);
	}

	// count how many controls are active
	int32 NumActiveControls = 0;

	// figure out how much to scale the control sizes
	float ScaleFactor = GetScaleFactor(AllottedGeometry);

	for (int32 ControlIndex = 0; ControlIndex < MyControls.Num(); ControlIndex++)
	{
		FTPTControlInfo& Control = MyControls[ControlIndex];

		if (Control.bNeedUpdatedCenter)
		{
			Control.ElapsedTime += InDeltaTime;
			if (Control.ElapsedTime > ActivationDelay)
			{
				Control.bNeedUpdatedCenter = false;
				CurrentOpacity = ActiveOpacity;

				if (!bPreventReCenter)
				{
					Control.VisualCenter = Control.NextCenter;
				}

				HandleTouch(ControlIndex, Control.NextCenter, AllottedGeometry.GetLocalSize());
			}
		}

		// calculate absolute positions based on geometry
		// @todo: Need to manage geometry changing!
		FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(Owner.Get());
		if (!Control.bHasBeenPositioned || ViewportSize != PreviousViewportSize)
		{
			FMargin Padding;
			PreviousViewportSize = ViewportSize;
			FSlateApplication::Get().GetSafeZoneSize(Padding, ViewportSize);

			// LM-project constant of safe area Top zero and Bottom zero
			Padding.Top = 0.0f;
			Padding.Bottom = 0.0f;

			// LM-project constant of safe area scale
			FMargin ComputedMargin = ComputeScaledSafeMargin(Padding, AllottedGeometry.Scale) * FMargin(0.7f, 1.0f);

			float ControlCenterX = Control.Center.X + ComputedMargin.Left;
			float ControlCenterY = Control.Center.Y + ComputedMargin.Bottom;

			// update all the sizes
			Control.CorrectedCenter = FVector2D(ResolveRelativePosition(ControlCenterX, AllottedGeometry.GetLocalSize().X, ScaleFactor), ResolveRelativePosition(ControlCenterY, AllottedGeometry.GetLocalSize().Y, ScaleFactor));
			Control.VisualCenter = Control.CorrectedCenter;
			Control.CorrectedVisualSize = FVector2D(ResolveRelativePosition(Control.VisualSize.X, AllottedGeometry.GetLocalSize().X, ScaleFactor), ResolveRelativePosition(Control.VisualSize.Y, AllottedGeometry.GetLocalSize().Y, ScaleFactor));
			Control.CorrectedInteractionSize = FVector2D(ResolveRelativePosition(Control.InteractionSize.X, AllottedGeometry.GetLocalSize().X, ScaleFactor), ResolveRelativePosition(Control.InteractionSize.Y, AllottedGeometry.GetLocalSize().Y, ScaleFactor));
			Control.CorrectedThumbSize = FVector2D(ResolveRelativePosition(Control.ThumbSize.X, AllottedGeometry.GetLocalSize().X, ScaleFactor), ResolveRelativePosition(Control.ThumbSize.Y, AllottedGeometry.GetLocalSize().Y, ScaleFactor));
			Control.CorrectedInputScale = Control.InputScale; // *ScaleFactor;
			Control.bHasBeenPositioned = true;
		}

		// is this active?
		if (Control.CapturedPointerIndex != -1)
		{
			NumActiveControls++;
		}
	}

	// we need to store the computed scale factor so we can compare it with the value computed in the following frame and, if necessary, recompute widget position
	PreviousScalingFactor = ScaleFactor;

	// STATE MACHINE!
	if (NumActiveControls > 0 || bPreventReCenter)
	{
		// any active control snaps the state to active immediately
		State = State_Active;
	}
	else
	{
		switch (State)
		{
		case State_WaitForStart:
		{
			State = State_CountingDownToStart;
			Countdown = StartupDelay;
		}
		break;
		case State_CountingDownToStart:
			// update the countdown
			Countdown -= InDeltaTime;
			if (Countdown <= 0.0f)
			{
				State = State_Inactive;
			}
			break;
		case State_Active:
			if (NumActiveControls == 0)
			{
				// start going to inactive
				State = State_CountingDownToInactive;
				Countdown = TimeUntilDeactive;
			}
			break;

		case State_CountingDownToInactive:
			// update the countdown
			Countdown -= InDeltaTime;
			if (Countdown <= 0.0f)
			{
				// should we start counting down to a control reset?
				if (TimeUntilReset > 0.0f)
				{
					State = State_CountingDownToReset;
					Countdown = TimeUntilReset;
				}
				else
				{
					// if not, then just go inactive
					State = State_Inactive;
				}
			}
			break;

		case State_CountingDownToReset:
			Countdown -= InDeltaTime;
			if (Countdown <= 0.0f)
			{
				// reset all the controls
				for (int32 ControlIndex = 0; ControlIndex < MyControls.Num(); ControlIndex++)
				{
					MyControls[ControlIndex].Reset();
				}

				// finally, go inactive
				State = State_Inactive;
			}
			break;
		}
	}
}

float STPTVirtualJoystick::GetBaseOpacity() const
{
	return (State == State_Active || State == State_CountingDownToInactive) ? ActiveOpacity : InactiveOpacity;
}

bool STPTVirtualJoystick::HandleTouch(int32 ControlIndex, const FVector2D& LocalCoord, const FVector2D& ScreenSize)
{
	FTPTControlInfo& Control = MyControls[ControlIndex];

	// figure out position around center
	FVector2D Offset = LocalCoord - MyControls[ControlIndex].VisualCenter;
	// only do work if we aren't at the center
	if (Offset == FVector2D(0, 0))
	{
		Control.ThumbPosition = Offset;
	}
	else
	{
		// clamp to the ellipse of the stick (snaps to the visual size, so, the art should go all the way to the edge of the texture)
		float DistanceToTouchSqr = Offset.SizeSquared();
		float Angle = FMath::Atan2(Offset.Y, Offset.X);

		// length along line to ellipse: L = 1.0 / sqrt(((sin(T)/Rx)^2 + (cos(T)/Ry)^2))
		float CosAngle = FMath::Cos(Angle);
		float SinAngle = FMath::Sin(Angle);
		float XTerm = CosAngle / (Control.CorrectedVisualSize.X * 0.5f);
		float YTerm = SinAngle / (Control.CorrectedVisualSize.Y * 0.5f);
		float DistanceToEdge = FMath::InvSqrt(XTerm * XTerm + YTerm * YTerm);

		// only clamp 
		if (DistanceToTouchSqr > FMath::Square(DistanceToEdge))
		{
			Control.ThumbPosition = FVector2D(DistanceToEdge * CosAngle, DistanceToEdge * SinAngle);
		}
		else
		{
			Control.ThumbPosition = Offset;
		}
	}

	FVector2D AbsoluteThumbPos = Control.ThumbPosition + MyControls[ControlIndex].VisualCenter;
	//AlignBoxIntoScreen(AbsoluteThumbPos, Control.CorrectedThumbSize, ScreenSize);
	Control.ThumbPosition = AbsoluteThumbPos - MyControls[ControlIndex].VisualCenter;

	return true;
}

FMargin STPTVirtualJoystick::ComputeScaledSafeMargin(const FMargin& SafeMargin, float Scale) const
{
	const float InvScale = 1.0f / Scale;

	const FMargin ScaledSafeMargin(
		FMath::RoundToFloat(SafeMargin.Left * InvScale),
		FMath::RoundToFloat(SafeMargin.Top * InvScale),
		FMath::RoundToFloat(SafeMargin.Right * InvScale),
		FMath::RoundToFloat(SafeMargin.Bottom * InvScale));
	return ScaledSafeMargin;
}