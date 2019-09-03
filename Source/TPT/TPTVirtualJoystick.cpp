// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTVirtualJoystick.h"
#include "SlateApplication.h"

void STPTVirtualJoystick::Construct(const FArguments& InArgs)
{
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
	FSlateApplication::Get().GetPlatformApplication()->OnDisplayMetricsChanged().AddSP(this, &SVirtualJoystick::HandleDisplayMetricsChanged);
}

FReply STPTVirtualJoystick::OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	FReply ReturnValue = SVirtualJoystick::OnTouchStarted(MyGeometry, Event);

	OnThumbMovementStarted.Broadcast();

	return ReturnValue;
}

FReply STPTVirtualJoystick::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	FReply ReturnValue = SVirtualJoystick::OnTouchMoved(MyGeometry, Event);

	OnThumbMovementDeltaFromCenter.Broadcast();

	return ReturnValue;
}

FReply STPTVirtualJoystick::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	FReply ReturnValue = SVirtualJoystick::OnTouchStarted(MyGeometry, Event);

	OnThumbMovementIsOver.Broadcast();

	return ReturnValue;
}

void STPTVirtualJoystick::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SVirtualJoystick::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

}
