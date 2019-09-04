// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTVirtualJoystick.h"
#include "SlateApplication.h"
#include "TPT.h"

void STPTVirtualJoystick::Construct(const FArguments& InArgs)
{
	bTouchStarted = false;
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
	FReply ReturnValue = SVirtualJoystick::OnTouchStarted(MyGeometry, Event);
	bTouchStarted = true;

	OnThumbMovementStarted.ExecuteIfBound();

	return ReturnValue;
}

FReply STPTVirtualJoystick::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	return SVirtualJoystick::OnTouchMoved(MyGeometry, Event);
}

FReply STPTVirtualJoystick::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	FReply ReturnValue = SVirtualJoystick::OnTouchEnded(MyGeometry, Event);
	
	bTouchStarted = false;
	LastThumbDelta = FVector2D::ZeroVector;
	LastThumbPosition = FVector2D::ZeroVector;

	OnThumbMovementIsOver.ExecuteIfBound();

	return ReturnValue;
}

bool STPTVirtualJoystick::HandleTouch(int32 ControlIndex, const FVector2D& LocalCoord, const FVector2D& ScreenSize)
{
	bool ReturnValue = SVirtualJoystick::HandleTouch(ControlIndex, LocalCoord, ScreenSize);

	if (bTouchStarted)
	{
		LastThumbDelta = LocalCoord - LastThumbPosition;

		float fDPIScale = LastThumbDelta.Size();
		TPT_LOG(Log, TEXT("%3.3f"), fDPIScale);
		if (fDPIScale > MoveThreshold)
		{
			OnThumbMovementDeltaFromCenter.ExecuteIfBound(LastThumbPosition, LastThumbDelta);
		}
	}
	else
	{
		LastThumbPosition = LocalCoord;
	}

	return ReturnValue;
}