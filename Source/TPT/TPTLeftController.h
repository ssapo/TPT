// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TFTWidget.h"
#include "DelegateCombinations.h"
#include "TPTLeftController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLeftControllerMovementOver);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLeftControllerEvent, const FVector2D&, InVector);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLeftControllerThumbDeltaEvent, const FVector2D&, InThumbLocation, const FVector2D&, InDelta);

/**
 * 
 */
UCLASS()
class TPT_API UTPTLeftController : public UTFTWidget
{
	GENERATED_BODY()
	
public:
	UTPTLeftController();

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;

	void CalculateThumLocAndDeltaFromNewCenter();

	FVector2D GetCursorLocation() const { return CursorLocation; }
	void SetCursorLocation(const FVector2D& InLocation);

	UPROPERTY()
		FVector2D CursorLocation;

	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;

	FVector2D GetScreenSize() const;

	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;

	void ProcessTouchEnd(const FPointerEvent &InGestureEvent);

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdateThumbLocationAndDelta();

	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

protected:
	UFUNCTION()
		void CallBack_TouchMovement(FVector InDelta, FVector InLocation);

protected:
	UPROPERTY(Transient)
		bool TouchStarted;

	UPROPERTY(Transient)
		bool FirstTick;

	UPROPERTY(Transient)
		FVector2D NewTouchCenterLocation;

	UPROPERTY(Transient)
		FVector2D LastThumbDelta;

	UPROPERTY(Transient)
		FVector2D ThumbLocation;

	UPROPERTY(EditDefaultsOnly)
		float ThumbMaxRadius;

	UPROPERTY(EditDefaultsOnly)
		float MoveThreshold;

	UPROPERTY()
		int32 FingerIndex;

	UPROPERTY()
		FVector2D DefaultThumbLocation;

	UPROPERTY()
		FVector2D DefaultControllerLocation;

	void ResetThumbLocation();


protected:
	UPROPERTY(meta = (BindWidget))
		class UImage* Image_ThumbCenter;

	UPROPERTY(meta = (BindWidget))
		class UImage* Image_Thumb;

	UPROPERTY(meta = (BindWidget))
		class UBorder* Border_Controller;

	UPROPERTY(meta = (BindWidget))
		UPanelWidget* Canvas_InputCheck;

public:
	/*UPROPERTY(BlueprintAssignable)
		FLeftControllerEvent OnThumbLocationUpdated;*/

	UPROPERTY(BlueprintAssignable)
		FLeftControllerMovementOver OnThumbMovementStarted;

	UPROPERTY(BlueprintAssignable)
		FLeftControllerThumbDeltaEvent OnThumbMovementDeltaFromCenter;

	UPROPERTY(BlueprintAssignable)
		FLeftControllerMovementOver OnThumbMovementIsOver;
};
