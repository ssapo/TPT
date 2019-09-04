// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/SlateDelegates.h"
#include "SVirtualJoystick.h"

DECLARE_DELEGATE_TwoParams(FThumbDeltaEvent, const FVector2D&, const FVector2D&);

class TPT_API STPTVirtualJoystick : public SVirtualJoystick
{
public:
	SLATE_BEGIN_ARGS(STPTVirtualJoystick)
	{}

	SLATE_EVENT(FSimpleDelegate, OnMovementStart)

	SLATE_EVENT(FSimpleDelegate, OnMovementOver)

	SLATE_EVENT(FThumbDeltaEvent, OnThumbDeltaEvent)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event) override;
	virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event) override;
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event) override;

	virtual bool HandleTouch(int32 ControlIndex, const FVector2D& LocalCoord, const FVector2D& ScreenSize) override;

public:
	FSimpleDelegate OnThumbMovementStarted;

	FSimpleDelegate OnThumbMovementIsOver;
	
	FThumbDeltaEvent OnThumbMovementDeltaFromCenter;


private:
	bool bTouchStarted;

	FVector2D LastThumbDelta;

	FVector2D LastThumbPosition;

	float MoveThreshold;
};
