// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTVirtualJoystick.h"
#include "SlateApplication.h"
#include "TPT.h"

static FORCEINLINE float GetScaleFactor(const FGeometry& Geometry)
{
	const float DesiredWidth = 1280.0f;

	float UndoDPIScaling = 1.0f / Geometry.Scale;
	return (Geometry.GetDrawSize().GetMax() / DesiredWidth) * UndoDPIScaling;
}

void STPTVirtualJoystick::Construct(const FArguments& InArgs)
{
	bTouchStarted = false;
	bHoldingTouch = false;

	LastThumbDelta = FVector2D::ZeroVector;
	LastThumbPosition = FVector2D::ZeroVector;
	MoveThreshold = 10.0f;

	OnThumbMovementStarted = InArgs._OnMovementStart;
	OnThumbMovementDeltaFromCenter = InArgs._OnThumbDeltaEvent;
	OnThumbMovementIsOver = InArgs._OnMovementOver;

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
	if (bTouchStarted)
	{
		return FReply::Unhandled();
	}

	FReply ReturnValue = SVirtualJoystick::OnTouchStarted(MyGeometry, Event);

	//if (bTouchStarted == false)
	//{
		bTouchStarted = true;
		//bHoldingTouch = true;

		OnThumbMovementStarted.ExecuteIfBound();
	//}

	return ReturnValue;
}

FReply STPTVirtualJoystick::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	return SVirtualJoystick::OnTouchMoved(MyGeometry, Event);
}

FReply STPTVirtualJoystick::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	FReply ReturnValue = SVirtualJoystick::OnTouchEnded(MyGeometry, Event);
	
	if (bTouchStarted)
	{
		bTouchStarted = false;
		bHoldingTouch = false;

		OnThumbMovementIsOver.ExecuteIfBound();

		LastThumbDelta = FVector2D::ZeroVector;
		LastThumbPosition = FVector2D::ZeroVector;
	}

	return ReturnValue;
}

bool STPTVirtualJoystick::HandleTouch(int32 ControlIndex, const FVector2D& LocalCoord, const FVector2D& ScreenSize)
{
	bool ReturnValue = SVirtualJoystick::HandleTouch(ControlIndex, LocalCoord, ScreenSize);

	if (bTouchStarted)
	{
		LastThumbDelta = (ThumbPosition - LocalCoord);

		float MoveSize = ((ThumbPosition - LastThumbPosition) - (ThumbPosition - LocalCoord)).Size();
		LastThumbPosition = LocalCoord;
		
		//TPT_LOG(Log, TEXT("%3.3f"), fDPIScale);
		if (MoveSize > MoveThreshold)
		{
			OnThumbMovementDeltaFromCenter.ExecuteIfBound(LastThumbPosition, LastThumbDelta);
		}
	}
	else
	{
		ThumbPosition = LocalCoord;
		LastThumbPosition = ThumbPosition;
	}

	return ReturnValue;
}

void STPTVirtualJoystick::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SVirtualJoystick::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}
