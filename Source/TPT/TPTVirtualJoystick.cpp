// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTVirtualJoystick.h"
#include "SlateApplication.h"
#include "TPT.h"
#include "SlateBlueprintLibrary.h"
#include "TFTPC.h"
#include "WidgetLayoutLibrary.h"

// -------------------------------------------------------------------------------------------------
const float OPACITY_LERP_RATE = 3.f;

// -------------------------------------------------------------------------------------------------
static FORCEINLINE float GetScaleFactor(const FGeometry& Geometry)
{
	const float DesiredWidth = 1280.0f;

	float UndoDPIScaling = 1.0f / Geometry.Scale;
	return (Geometry.GetDrawSize().GetMax() / DesiredWidth) * UndoDPIScaling;
}

static FORCEINLINE int32 ResolveRelativePosition(float Position, float RelativeTo, float ScaleFactor)
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

static FORCEINLINE bool PositionIsInside(const FVector2D& Center, const FVector2D& Position, const FVector2D& BoxSize
)
{
	return
		Position.X >= Center.X - BoxSize.X * 0.5f &&
		Position.X <= Center.X + BoxSize.X * 0.5f &&
		Position.Y >= Center.Y - BoxSize.Y * 0.5f &&
		Position.Y <= Center.Y + BoxSize.Y * 0.5f;
}

// -------------------------------------------------------------------------------------------------
void STPTVirtualJoystick::FControlInfo::Reset()
{
	// snap the visual center back to normal (for controls that have a center on touch)
	VisualCenter = CorrectedCenter;
}

// -------------------------------------------------------------------------------------------------
void STPTVirtualJoystick::Construct(const FArguments& InArgs)
{
	// ---------------------------------- Left Controller ------------------------------------------
	bTouchStarted = false;
	bFirstTick = true;

	LastThumbDelta = FVector2D::ZeroVector;
	ThumbLocation = FVector2D::ZeroVector;
	FingerIndex = -1;
	ThumbMaxRadius = 50.0f;

	// ------------------------------ VirtualJoystick Arguments ------------------------------------
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
	if (bVisible == false || Owner.IsValid() == false)
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

FReply STPTVirtualJoystick::Super_OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
//	UE_LOG(LogTemp, Log, TEXT("Pointer index: %d"), Event.GetPointerIndex());
	
	FVector2D LocalCoord = MyGeometry.AbsoluteToLocal(Event.GetScreenSpacePosition());

	for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
	{
		FControlInfo& Control = Controls[ControlIndex];

		// skip controls already in use
		if (Control.CapturedPointerIndex == -1)
		{
			if (PositionIsInside(Control.CorrectedCenter, LocalCoord, Control.CorrectedInteractionSize))
			{
				// Align Joystick inside of Screen
				AlignBoxIntoScreen(LocalCoord, Control.CorrectedVisualSize, MyGeometry.GetLocalSize());

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

FReply STPTVirtualJoystick::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	if (bVisible == false || Owner.IsValid() == false)
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

FReply STPTVirtualJoystick::Super_OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	FVector2D LocalCoord = MyGeometry.AbsoluteToLocal(Event.GetScreenSpacePosition());

	for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
	{
		FControlInfo& Control = Controls[ControlIndex];

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

FReply STPTVirtualJoystick::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	if (bVisible == false || Owner.IsValid() == false)
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

FReply STPTVirtualJoystick::Super_OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
	{
		FControlInfo& Control = Controls[ControlIndex];

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

void STPTVirtualJoystick::Tick(
	const FGeometry& AllottedGeometry, 
	const double InCurrentTime, 
	const float InDeltaTime
)
{
	if (bVisible == false || Owner.IsValid() == false)
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

	for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
	{
		FControlInfo& Control = Controls[ControlIndex];

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
		if (!Control.bHasBeenPositioned || ScaleFactor != PreviousScalingFactor)
		{
			// update all the sizes
			Control.CorrectedCenter = FVector2D(ResolveRelativePosition(Control.Center.X, AllottedGeometry.GetLocalSize().X, ScaleFactor), ResolveRelativePosition(Control.Center.Y, AllottedGeometry.GetLocalSize().Y, ScaleFactor));
			Control.VisualCenter = Control.CorrectedCenter;
			Control.CorrectedVisualSize = FVector2D(ResolveRelativePosition(Control.VisualSize.X, AllottedGeometry.GetLocalSize().X, ScaleFactor), ResolveRelativePosition(Control.VisualSize.Y, AllottedGeometry.GetLocalSize().Y, ScaleFactor));
			Control.CorrectedInteractionSize = FVector2D(ResolveRelativePosition(Control.InteractionSize.X, AllottedGeometry.GetLocalSize().X, ScaleFactor), ResolveRelativePosition(Control.InteractionSize.Y, AllottedGeometry.GetLocalSize().Y, ScaleFactor));
			Control.CorrectedThumbSize = FVector2D(ResolveRelativePosition(Control.ThumbSize.X, AllottedGeometry.GetLocalSize().X, ScaleFactor), ResolveRelativePosition(Control.ThumbSize.Y, AllottedGeometry.GetLocalSize().Y, ScaleFactor));
			Control.CorrectedInputScale = Control.InputScale; // *ScaleFactor;
			Control.bHasBeenPositioned = true;
		}

		if (Control.CapturedPointerIndex >= 0 || Control.bSendOneMoreEvent)
		{
			Control.bSendOneMoreEvent = false;

			// Get the corrected thumb offset scale (now allows ellipse instead of assuming square)
			FVector2D ThumbScaledOffset = FVector2D(Control.ThumbPosition.X * 2.0f / Control.CorrectedVisualSize.X, Control.ThumbPosition.Y * 2.0f / Control.CorrectedVisualSize.Y);
			float ThumbSquareSum = ThumbScaledOffset.X * ThumbScaledOffset.X + ThumbScaledOffset.Y * ThumbScaledOffset.Y;
			float ThumbMagnitude = FMath::Sqrt(ThumbSquareSum);
			FVector2D ThumbNormalized = FVector2D(0.f, 0.f);
			if (ThumbSquareSum > SMALL_NUMBER)
			{
				const float Scale = 1.0f / ThumbMagnitude;
				ThumbNormalized = FVector2D(ThumbScaledOffset.X * Scale, ThumbScaledOffset.Y * Scale);
			}

			// Find the scale to apply to ThumbNormalized vector to project onto unit square
			float ToSquareScale = fabs(ThumbNormalized.Y) > fabs(ThumbNormalized.X) ? FMath::Sqrt((ThumbNormalized.X * ThumbNormalized.X) / (ThumbNormalized.Y * ThumbNormalized.Y) + 1.0f)
				: ThumbNormalized.X == 0.0f ? 1.0f : FMath::Sqrt((ThumbNormalized.Y * ThumbNormalized.Y) / (ThumbNormalized.X * ThumbNormalized.X) + 1.0f);

			// Apply proportional offset corrected for projection to unit square
			FVector2D NormalizedOffset = ThumbNormalized * Control.CorrectedInputScale * ThumbMagnitude * ToSquareScale;

			// now pass the fake joystick events to the game
			const FGamepadKeyNames::Type XAxis = (Control.MainInputKey.IsValid() ? Control.MainInputKey.GetFName() : (ControlIndex == 0 ? FGamepadKeyNames::LeftAnalogX : FGamepadKeyNames::RightAnalogX));
			const FGamepadKeyNames::Type YAxis = (Control.AltInputKey.IsValid() ? Control.AltInputKey.GetFName() : (ControlIndex == 0 ? FGamepadKeyNames::LeftAnalogY : FGamepadKeyNames::RightAnalogY));

			//FSlateApplication::Get().SetAllUserFocusToGameViewport();
			FSlateApplication::Get().OnControllerAnalog(XAxis, 0, NormalizedOffset.X);
			FSlateApplication::Get().OnControllerAnalog(YAxis, 0, -NormalizedOffset.Y);
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
				for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
				{
					Controls[ControlIndex].Reset();
				}

				// finally, go inactive
				State = State_Inactive;
			}
			break;
		}
	}
}

int32 STPTVirtualJoystick::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	return Super_OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

int32 STPTVirtualJoystick::Super_OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 RetLayerId = LayerId;

	if (bVisible)
	{
		FLinearColor ColorAndOpacitySRGB = InWidgetStyle.GetColorAndOpacityTint();
		ColorAndOpacitySRGB.A = CurrentOpacity;

		for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
		{
			const FControlInfo& Control = Controls[ControlIndex];

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

void STPTVirtualJoystick::AddControl(const FControlInfo& Control)
{
	Controls.Add(Control);
}

void STPTVirtualJoystick::ClearControls()
{
	Controls.Empty();
}

void STPTVirtualJoystick::SetControls(const TArray<FControlInfo>& InControls)
{
	Controls = InControls;
}

void STPTVirtualJoystick::AlignBoxIntoScreen(FVector2D& Position, const FVector2D& Size, const FVector2D& ScreenSize)
{
	FVector2D ExstraSize = ScreenSize * 0.02f;
	FVector2D MaxScreen = ScreenSize + ExstraSize;
	FVector2D MinScreen = -ExstraSize;

	if (Size.X < MinScreen.X || Size.Y < MinScreen.Y)
	{
		return;
	}

	if (Size.X > MaxScreen.X || Size.Y > MaxScreen.Y)
	{
		return;
	}

	// Align box to fit into screen
	if (Position.X - Size.X * 0.5f < MinScreen.X)
	{
		Position.X = MinScreen.X + Size.X * 0.5f;
	}

	if (Position.X + Size.X * 0.5f > MaxScreen.X)
	{
		Position.X = MaxScreen.X - Size.X * 0.5f;
	}

	if (Position.Y - Size.Y * 0.5f < MinScreen.Y)
	{
		Position.Y = MinScreen.Y + Size.Y * 0.5f;
	}

	if (Position.Y + Size.Y * 0.5f > MaxScreen.Y)
	{
		Position.Y = MaxScreen.Y - Size.Y * 0.5f;
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
