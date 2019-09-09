// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SVirtualJoystick.h"
#include "Framework/SlateDelegates.h"

class FPaintArgs;
class FSlateWindowElementList;

DECLARE_DELEGATE_TwoParams(FThumbDeltaEvent, const FVector2D&, const FVector2D&);

class TPT_API STPTVirtualJoystick : public SVirtualJoystick
{
public:
	struct FControlInfo
	{
		FControlInfo()
		{
			FMemory::Memzero(this, sizeof(*this));
			CapturedPointerIndex = -1;
			InputScale = FVector2D(1.f, 1.f);
		}
		TSharedPtr< ISlateBrushSource > Image1;
		TSharedPtr< ISlateBrushSource > Image2;
		FVector2D Center;
		FVector2D VisualSize;
		FVector2D ThumbSize;
		FVector2D InteractionSize;
		FVector2D InputScale;
		FKey MainInputKey;
		FKey AltInputKey;
		FVector2D PositionedCenter;

	private:
		friend STPTVirtualJoystick;

		void Reset();
		FVector2D ThumbPosition;
		FVector2D VisualCenter;
		FVector2D CorrectedCenter;
		FVector2D CorrectedVisualSize;
		FVector2D CorrectedThumbSize;
		FVector2D CorrectedInteractionSize;
		FVector2D CorrectedInputScale;
		int32 CapturedPointerIndex;
		float ElapsedTime;
		FVector2D NextCenter;
		bool bSendOneMoreEvent;
		bool bHasBeenPositioned;
		bool bNeedUpdatedCenter;
	};

	SLATE_BEGIN_ARGS(STPTVirtualJoystick)
	{}

	SLATE_ARGUMENT(class ATFTPC*, Owner)

	SLATE_EVENT(FSimpleDelegate, OnMovementStart)

	SLATE_EVENT(FSimpleDelegate, OnMovementOver)

	SLATE_EVENT(FThumbDeltaEvent, OnThumbDeltaEvent)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	/**
	 * Sets parameters that control all controls
	 */

	virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InEvent) override;
	FReply Super_OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InEvent);
	
	virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InEvent) override;
	FReply Super_OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InEvent);
	
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InEvent) override;
	FReply Super_OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InEvent);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	void Super_Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	int32 Super_OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;

protected:
	virtual void AlignBoxIntoScreen(FVector2D& Position, const FVector2D& Size, const FVector2D& ScreenSize);
	virtual void CalculateThumLocAndDeltaFromNewCenter(const FGeometry& MyGeometry);
	virtual void UpdateThumbLocationAndDelta();

private:

	

public:
	void AddControl(const FControlInfo& Control);

	void ClearControls();

	void SetControls(const TArray<FControlInfo>& InControls);

public:
	FSimpleDelegate OnThumbMovementStarted;

	FSimpleDelegate OnThumbMovementIsOver;
	
	FThumbDeltaEvent OnThumbMovementDeltaFromCenter;

protected:
	TWeakObjectPtr<class ATFTPC> Owner;
	
	TArray<FControlInfo> Controls;

	FVector2D LastThumbDelta;

	FVector2D ThumbLocation;
	
	FVector2D NewTouchCenterLocation;

	FVector2D CursorLocation;

	float ThumbMaxRadius;

	int32 FingerIndex;

	bool bTouchStarted;

	bool bFirstTick;
};
