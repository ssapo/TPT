#pragma once

#include "CoreMinimal.h"
#include "Framework/SlateDelegates.h"
#include "SVirtualJoystick.h"

DECLARE_DELEGATE_TwoParams(FThumbDeltaEvent, const FVector2D&, const FVector2D&);

class TPT_API STPTVirtualJoystick : public SVirtualJoystick
{
private:
	struct FTPTControlInfo
	{
		FTPTControlInfo()
		{
			// default to all 0
			FMemory::Memzero(this, sizeof(*this));
			CapturedPointerIndex = -1;
			InputScale = FVector2D(1.f, 1.f);
		}

		// Set by the game

		/** The brush to use to draw the background for joysticks, or unclicked for buttons */
		TSharedPtr< ISlateBrushSource > Image1;

		/** The brush to use to draw the thumb for joysticks, or clicked for buttons */
		TSharedPtr< ISlateBrushSource > Image2;

		/** The actual center of the control */
		FVector2D Center;

		/** The size of a joystick that can be re-centered within InteractionSize area */
		FVector2D VisualSize;

		/** The size of the thumb that can be re-centered within InteractionSize area */
		FVector2D ThumbSize;

		/** The size of a the interactable area around Center */
		FVector2D InteractionSize;

		/** The scale for control input */
		FVector2D InputScale;

		/** The input to send from this control (for sticks, this is the horizontal/X input) */
		FKey MainInputKey;

		/** The secondary input (for sticks, this is the vertical/Y input, unused for buttons) */
		FKey AltInputKey;

		/** Positioned center in viewport */
		FVector2D PositionedCenter;

	private:
		friend STPTVirtualJoystick;

		/**
		 * Reset the control to a centered/inactive state
		 */
		void Reset();

		// Current state

		/** The position of the thumb, in relation to the VisualCenter */
		FVector2D ThumbPosition;

		/** For recentered joysticks, this is the re-center location */
		FVector2D VisualCenter;

		/** The corrected actual center of the control */
		FVector2D CorrectedCenter;

		/** The corrected size of a joystick that can be re-centered within InteractionSize area */
		FVector2D CorrectedVisualSize;

		/** The corrected size of the thumb that can be re-centered within InteractionSize area */
		FVector2D CorrectedThumbSize;

		/** The corrected size of a the interactable area around Center */
		FVector2D CorrectedInteractionSize;

		/** The corrected scale for control input */
		FVector2D CorrectedInputScale;

		/** Which pointer index is interacting with this control right now, or -1 if not interacting */
		int32 CapturedPointerIndex;

		/** Time to activate joystick **/
		float ElapsedTime;

		/** Visual center to be updated */
		FVector2D NextCenter;

		/** Whether or not to send one last "release" event next tick */
		bool bSendOneMoreEvent;

		/** Whether or not we need position the control against the geometry */
		bool bHasBeenPositioned;

		/** Whether or not to update center position */
		bool bNeedUpdatedCenter;
	};

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
	
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	void CopyControls();

protected:
	virtual bool HandleTouch(int32 ControlIndex, const FVector2D& LocalCoord, const FVector2D& ScreenSize) override;

private:
	void CalculateThumLocAndDeltaFromNewCenter(const FGeometry& MyGeometry);
	void UpdateThumbLocationAndDelta();

	FReply Super_OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event);
	FReply Super_OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event);
	FReply Super_OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event);
	void Super_Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);

	float LMGetBaseOpacity() const;
	FMargin LMComputeScaledSafeMargin(const FMargin& InMargin, float InScale) const;

public:
	FSimpleDelegate OnThumbMovementStarted;

	FSimpleDelegate OnThumbMovementIsOver;
	
	FThumbDeltaEvent OnThumbMovementDeltaFromCenter;

private:
	TWeakObjectPtr<class ATFTPC> Owner;
	
	TArray<FTPTControlInfo> MyControls;

	FVector2D LastThumbDelta;

	FVector2D ThumbLocation;
	
	FVector2D NewTouchCenterLocation;

	FVector2D CursorLocation;

	FVector2D PreviousViewportSize;

	float ThumbMaxRadius;

	int32 FingerIndex;

	bool bTouchStarted;

	bool bFirstTick;
};
