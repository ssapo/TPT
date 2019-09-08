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

	SLATE_ARGUMENT(class ATFTPC*, Owner)

	SLATE_EVENT(FSimpleDelegate, OnMovementStart)

	SLATE_EVENT(FSimpleDelegate, OnMovementOver)

	SLATE_EVENT(FThumbDeltaEvent, OnThumbDeltaEvent)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InEvent) override;
	virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InEvent) override;
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InEvent) override;

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	virtual FVector2D GetExtraSizeForIntoScreen() override;

private:
	void CalculateThumLocAndDeltaFromNewCenter(const FGeometry& MyGeometry);
	void UpdateThumbLocationAndDelta();

public:
	FSimpleDelegate OnThumbMovementStarted;

	FSimpleDelegate OnThumbMovementIsOver;
	
	FThumbDeltaEvent OnThumbMovementDeltaFromCenter;

private:
	TWeakObjectPtr<class ATFTPC> Owner;

	FVector2D LastThumbDelta;

	FVector2D ThumbLocation;
	
	FVector2D NewTouchCenterLocation;

	FVector2D CursorLocation;

	float ThumbMaxRadius;

	int32 FingerIndex;

	bool bTouchStarted;

	bool bFirstTick;
};
