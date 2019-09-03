// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DelegateCombinations.h"
#include "SVirtualJoystick.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTPTVirtualJoystickMovementOver);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTPTVirtualJoystickEvent, const FVector2D&, InVector);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTPTVirtualJoystickThumbDeltaEvent, const FVector2D&, InThumbLocation, const FVector2D&, InDelta);

class TPT_API STPTVirtualJoystick : public SVirtualJoystick
{
	GENERATED_BODY()

public:
	SLATE_BEGIN_ARGS(STPTVirtualJoystick)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event) override;
	virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event) override;
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event) override;


	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

public:
	UPROPERTY(BlueprintAssignable)
		FTPTVirtualJoystickMovementOver OnThumbMovementStarted;

	UPROPERTY(BlueprintAssignable)
		FTPTVirtualJoystickEvent OnThumbMovementDeltaFromCenter;

	UPROPERTY(BlueprintAssignable)
		FTPTVirtualJoystickMovementOver OnThumbMovementIsOver;


};
