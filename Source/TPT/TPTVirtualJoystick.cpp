// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTVirtualJoystick.h"
#include "SlateApplication.h"
#include "TPT.h"
#include "SlateBlueprintLibrary.h"
#include "TFTPC.h"
#include "WidgetLayoutLibrary.h"

static FORCEINLINE float GetScaleFactor(const FGeometry& Geometry)
{
	const float DesiredWidth = 1280.0f;

	float UndoDPIScaling = 1.0f / Geometry.Scale;
	return (Geometry.GetDrawSize().GetMax() / DesiredWidth) * UndoDPIScaling;
}

void STPTVirtualJoystick::Construct(const FArguments& InArgs)
{
	// ---------------------------------- Left Controller ------------------------------------------
	bTouchStarted = false;

	LastThumbDelta = FVector2D::ZeroVector;
	ThumbLocation = FVector2D::ZeroVector;
	MoveThreshold = 10.0f;
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
	if (Owner.IsValid() == false)
	{
		return FReply::Unhandled();
	}

	if (FingerIndex == -1)
	{
		bTouchStarted = true;

		FVector2D PixelPosition, ViewportPosition;
		USlateBlueprintLibrary::AbsoluteToViewport(Owner.Get(), Event.GetScreenSpacePosition(),
			PixelPosition,
			ViewportPosition);

		CursorLocation = ViewportPosition;
		FingerIndex = Event.GetPointerIndex();
		NewCenterLocation = CursorLocation;

		OnThumbMovementStarted.ExecuteIfBound();
	}

	return SVirtualJoystick::OnTouchStarted(MyGeometry, Event);
}

FReply STPTVirtualJoystick::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	if (Owner.IsValid() == false)
	{
		return FReply::Unhandled();
	}

	if (FingerIndex == Event.GetPointerIndex())
	{
		FVector2D PixelPosition, ViewportPosition;
		USlateBlueprintLibrary::AbsoluteToViewport(Owner.Get(), Event.GetScreenSpacePosition(),
			PixelPosition,
			ViewportPosition);

		CursorLocation = ViewportPosition;
	}

	return SVirtualJoystick::OnTouchMoved(MyGeometry, Event);
}

FReply STPTVirtualJoystick::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	if (Owner.IsValid() == false)
	{
		return FReply::Unhandled();
	}

	if (FingerIndex == Event.GetPointerIndex())
	{
		FingerIndex = -1;

		bTouchStarted = false;

		LastThumbDelta = FVector2D::ZeroVector;
		ThumbLocation = FVector2D::ZeroVector;

		OnThumbMovementIsOver.ExecuteIfBound();
	}

	return SVirtualJoystick::OnTouchEnded(MyGeometry, Event);
}

void STPTVirtualJoystick::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (Owner.IsValid() == false)
	{
		return;
	}

	SVirtualJoystick::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (bTouchStarted)
	{
		CalculateThumLocAndDeltaFromNewCenter(AllottedGeometry);
		UpdateThumbLocationAndDelta();

		if (LastThumbDelta.IsNearlyZero() == false)
		{
			OnThumbMovementDeltaFromCenter.ExecuteIfBound(ThumbLocation, LastThumbDelta);
		}
		else 
		{
			OnThumbMovementIsOver.ExecuteIfBound();
		}
	}
}

void STPTVirtualJoystick::CalculateThumLocAndDeltaFromNewCenter(const FGeometry& MyGeometry)
{
	LastThumbDelta = CursorLocation - NewCenterLocation;

	float fDPIScale = UWidgetLayoutLibrary::GetViewportScale(Owner.Get());
	fDPIScale *= LastThumbDelta.Size();

	if (fDPIScale <= MoveThreshold)
	{
		LastThumbDelta = FVector2D::ZeroVector;
	}

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

	ThumbLocation = NewCenterLocation + DeltaNorm;
}
