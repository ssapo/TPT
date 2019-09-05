// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTLeftController.h"
#include "TFTPC.h"
#include "TPTTouchInputComponent.h"
#include "Image.h"
#include "Border.h"
#include "WidgetLayoutLibrary.h"
#include "SlateBlueprintLibrary.h"
#include "CanvasPanelSlot.h"
#include "TPT.h"
#include "SWidget.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "Geometry.h"
#include "SharedPointer.h"
#include "SViewport.h"


UTPTLeftController::UTPTLeftController()
{
	ThumbMaxRadius = 50.0f;
	FingerIndex = -1;
	MoveThreshold = 10.0f;
}

void UTPTLeftController::NativeConstruct()
{
	Super::NativeConstruct();

	TouchStarted = false;
	FirstTick = true;

	if (auto PC = this->GetPlayerContext().GetPlayerController<ATFTPC>())
	{
		if (auto TouchComponent = PC->GetTouchInputComponent())
		{
			TouchComponent->OnTouchMoveEvent.AddDynamic(this, &UTPTLeftController::CallBack_TouchMovement);
		}

		OnThumbMovementStarted.AddDynamic(PC, &ATFTPC::OnLeftControllerTouchStarted);
		OnThumbMovementDeltaFromCenter.AddDynamic(PC, &ATFTPC::OnLeftControllerTouched);
		OnThumbMovementIsOver.AddDynamic(PC, &ATFTPC::OnLeftControllerTouchedOver);
	}

	if (UCanvasPanelSlot* AsCanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Border_Controller))
	{
		DefaultControllerLocation = AsCanvasSlot->GetPosition();
	}

	if (UCanvasPanelSlot* AsCanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Image_Thumb))
	{
		DefaultThumbLocation = AsCanvasSlot->GetPosition();
	}
}

FReply UTPTLeftController::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	Super::NativeOnTouchMoved(InGeometry, InGestureEvent);

	if (FingerIndex == InGestureEvent.GetPointerIndex())
	{
		SetCursorLocation(InGestureEvent.GetScreenSpacePosition());
	}

	return FReply::Unhandled();
}

void UTPTLeftController::CalculateThumLocAndDeltaFromNewCenter()
{
	if (false == TouchStarted)
		return;

	if (APlayerController* PC = this->GetPlayerContext().GetPlayerController())
	{
		// calculate delta.
		{
			LastThumbDelta = GetCursorLocation() - NewTouchCenterLocation;

			// make zero unless if it is under threshold.
			{
				float fDPIScale = UWidgetLayoutLibrary::GetViewportScale(this);
				fDPIScale *= LastThumbDelta.Size();
				
				if (fDPIScale <= MoveThreshold)
				{
					LastThumbDelta = FVector2D::ZeroVector;
				}
			}
		}

		// calculate thumb-location.
		{
			FVector2D PixelPosition;
			FVector2D ViewportPosition;
			USlateBlueprintLibrary::LocalToViewport(this->GetWorld(), Image_ThumbCenter->GetCachedGeometry(), FVector2D::ZeroVector,
				PixelPosition,
				ViewportPosition);

			ThumbLocation = ViewportPosition;
		}

	}
}

void UTPTLeftController::SetCursorLocation(const FVector2D& InLocation)
{
	FVector2D PixelPosition, ViewportPosition;
	USlateBlueprintLibrary::AbsoluteToViewport(this->GetWorld(), InLocation,
		PixelPosition,
		ViewportPosition);

	CursorLocation = ViewportPosition;
}

FReply UTPTLeftController::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	if (FingerIndex == -1)
	{
		if (USlateBlueprintLibrary::IsUnderLocation(Border_Controller->GetCachedGeometry(), InGestureEvent.GetScreenSpacePosition()))
		{
			SetCursorLocation(InGestureEvent.GetScreenSpacePosition());

			FingerIndex = InGestureEvent.GetPointerIndex();
			TPT_PRINT_C(FColor::Red, TEXT("TouchStarted FingerIndex = %d"), FingerIndex);

			TouchStarted = true;
			FirstTick = false;

			// thumb location on center.		
			UpdateThumbLocationAndDelta();

			NewTouchCenterLocation = GetCursorLocation();

			LastThumbDelta = FVector2D::ZeroVector;

			Canvas_InputCheck->SetVisibility(ESlateVisibility::Visible);

			FVector2D ScreenTouchLocation = NewTouchCenterLocation;
			ScreenTouchLocation.Y -= GetScreenSize().Y;

			if (UCanvasPanelSlot* AsCanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Border_Controller))
			{
				AsCanvasSlot->SetPosition(ScreenTouchLocation);
			}

			OnThumbMovementStarted.Broadcast();
		}
	}

	return Super::NativeOnTouchStarted(InGeometry, InGestureEvent);
}

FVector2D UTPTLeftController::GetScreenSize() const
{
	FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this->GetWorld());
	FVector2D LeftTopPixelPosition, LeftBottomViewportPosition;

	auto World = this->GetWorld();
	TPT_CHECK(World != nullptr, FVector2D::ZeroVector);

	auto Viewport = World->GetGameViewport();
	TPT_CHECK(Viewport != nullptr, FVector2D::ZeroVector);

	auto ViewportWidget = Viewport->GetGameViewportWidget();
	TPT_CHECK(ViewportWidget.IsValid() != false, FVector2D::ZeroVector);

	USlateBlueprintLibrary::LocalToViewport(World, ViewportWidget->GetCachedGeometry(), ViewportSize, LeftTopPixelPosition, LeftBottomViewportPosition);

	return LeftBottomViewportPosition;
}

FReply UTPTLeftController::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	ProcessTouchEnd(InGestureEvent);

	return Super::NativeOnTouchEnded(InGeometry, InGestureEvent);
}

void UTPTLeftController::ProcessTouchEnd(const FPointerEvent &InGestureEvent)
{
	if (FingerIndex == InGestureEvent.GetPointerIndex())
	{
		FingerIndex = -1;

		TouchStarted = false;

		OnThumbMovementIsOver.Broadcast();

		Canvas_InputCheck->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		ResetThumbLocation();
	}
}

void UTPTLeftController::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	ProcessTouchEnd(InMouseEvent);
}

void UTPTLeftController::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	CalculateThumLocAndDeltaFromNewCenter();
	UpdateThumbLocationAndDelta();

	if (TouchStarted)
	{
		if (LastThumbDelta.IsNearlyZero() == false)
			OnThumbMovementDeltaFromCenter.Broadcast(ThumbLocation, LastThumbDelta);
		else
			OnThumbMovementIsOver.Broadcast();
	}
}

void UTPTLeftController::UpdateThumbLocationAndDelta()
{
	if (FirstTick || !TouchStarted)
		return;

	FVector2D DeltaNorm = LastThumbDelta.GetSafeNormal();

	float DeltaLength = LastThumbDelta.Size();
	DeltaNorm *= FMath::Clamp(DeltaLength, 1.0f, ThumbMaxRadius);

	ThumbLocation = NewTouchCenterLocation + DeltaNorm;

	FVector2D ScreenTouchLocation = ThumbLocation;
	ScreenTouchLocation.Y -= GetScreenSize().Y;

	if (UCanvasPanelSlot* AsCanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Image_Thumb))
	{
		AsCanvasSlot->SetPosition(ScreenTouchLocation);
	}
}

void UTPTLeftController::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);

	//TouchStarted = false;

	OnThumbMovementIsOver.Broadcast();
}

void UTPTLeftController::CallBack_TouchMovement(FVector InDelta, FVector InLocation)
{
	//UpdateThumbLocationAndDelta(FVector2D(InLocation.X, InLocation.Y));
}

void UTPTLeftController::ResetThumbLocation()
{
	if (UCanvasPanelSlot* AsCanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Border_Controller))
	{
		AsCanvasSlot->SetPosition(DefaultControllerLocation);
	}

	if (UCanvasPanelSlot* AsCanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Image_Thumb))
	{
		AsCanvasSlot->SetPosition(DefaultThumbLocation);
	}
}
