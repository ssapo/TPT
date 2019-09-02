// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTTouchInputComponent.h"
#include "TFTPC.h"
#include "Engine/Engine.h"

UTPTTouchInputComponent::UTPTTouchInputComponent()
{
}

void UTPTTouchInputComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (ATFTPC* PC = Cast<ATFTPC>(this->GetOwner()))
	{
		TouchType = ETPTTouchType::None;

		PC->InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &UTPTTouchInputComponent::TouchBegin);
		PC->InputComponent->BindTouch(EInputEvent::IE_Released, this, &UTPTTouchInputComponent::TouchEnd);
		PC->InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &UTPTTouchInputComponent::TouchMove);
	}
}

void UTPTTouchInputComponent::TouchBegin(const ETouchIndex::Type FingerIndex, FVector Location)
{
	Location.Z = 0.f;
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const FVector2D TouchInvalidation = FVector2D(ViewportSize.X / 3.0f, ViewportSize.Y / 2.0f);

	if (Location.X < TouchInvalidation.X && Location.Y > TouchInvalidation.Y)
	{
		TouchData[FingerIndex].bPress = false;
	}
	else
	{
		TouchData[FingerIndex].BeginLocation = Location;
		TouchData[FingerIndex].bPress = true;

		CheckMultiTouch();
		OnTouchBeginEvent.Broadcast(Location);
	}
}

void UTPTTouchInputComponent::TouchMove(const ETouchIndex::Type FingerIndex, FVector Location)
{
	Location.Z = 0.f;
	if (TouchData[FingerIndex].bPress)
	{
		if (TouchType == ETPTTouchType::SingleTouch)
		{
			SetTouchDelta(FingerIndex, Location);
			TouchData[FingerIndex].BeginLocation = Location;
		}
		else
		{
			// Check PinchToZoom
			int32 PressCount = 0;
			int32 FirstPress = ETouchIndex::Touch10;
			int32 SecondPress = ETouchIndex::Touch10;

			for (int32 StartIndex = 0; StartIndex < ETouchIndex::Touch6; ++StartIndex)
			{
				if (TouchData[StartIndex].bPress)
				{
					if (FirstPress == ETouchIndex::Touch10)
					{
						FirstPress = StartIndex;
					}
					else
					{
						SecondPress = StartIndex;
					}

					++PressCount;

					if (PressCount == 2)
					{
						break;
					}
				}
			}


			if (PressCount == 2)
			{
				if (FingerIndex == FirstPress || FingerIndex == SecondPress)
				{
					TouchData[FingerIndex].BeginLocation = Location;

					float PinchLength = FVector::Dist2D(TouchData[FirstPress].BeginLocation, TouchData[SecondPress].BeginLocation);

					if (LastPinchLength == 0.f)
					{
						LastPinchLength = PinchLength;
					}
					float DeltaLength = LastPinchLength - PinchLength;


					if (FMath::Abs(DeltaLength) > 1.0f)
					{
						if (0 != DeltaLength)
						{
							OnTouchPinchEvent.Broadcast(DeltaLength);
						}
					
						LastPinchLength = PinchLength;
					}

				}
			}
		}
	}
}

void UTPTTouchInputComponent::SetTouchDelta(const ETouchIndex::Type FingerIndex, const FVector& Location)
{
	if (TouchData[FingerIndex].BeginLocation != Location)
	{
		FVector DeltaVector = (TouchData[FingerIndex].BeginLocation - Location) * TouchInputRotDelta;

		DeltaVector.X = -DeltaVector.X;
		DeltaVector.Y = -DeltaVector.Y;

		OnTouchMoveEvent.Broadcast(DeltaVector, Location);
	}
}

void UTPTTouchInputComponent::TouchEnd(const ETouchIndex::Type FingerIndex, FVector Location)
{
	Location.Z = 0.f;
	if (TouchData[FingerIndex].bPress)
	{
		TouchData[FingerIndex].BeginLocation = FVector::ZeroVector;
		TouchData[FingerIndex].Delta = FVector::ZeroVector;
		TouchData[FingerIndex].bPress = false;

		LastPinchLength = 0.f;

		OnTouchEndEvent.Broadcast(CheckMultiTouch());
	}
}

bool UTPTTouchInputComponent::CheckMultiTouch()
{
	bool bMultiTouch = false;
	int32 PressCount = 0;

	for (int32 StartIndex = 0; StartIndex < ETouchIndex::Touch6; ++StartIndex)
	{
		if (TouchData[StartIndex].bPress)
		{
			++PressCount;
		}
	}

	if (PressCount < 2)
	{
		TouchType = ETPTTouchType::SingleTouch;
	}
	else
	{
		TouchType = ETPTTouchType::MultiTouch;
	}

	// Check All Release
	if (PressCount < 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

