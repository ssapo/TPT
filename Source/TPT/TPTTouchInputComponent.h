// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPTTouchInputComponent.generated.h"

namespace ETPTTouchType
{
	UENUM()
	enum Type
	{
		None,
		SingleTouch,
		MultiTouch,
		Max,
	};
};

USTRUCT()
struct FTouchDataStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		FVector		BeginLocation;

	UPROPERTY()
		FVector		Delta;

	UPROPERTY()
		bool		bPress;

	FTouchDataStruct()
		: BeginLocation(FVector::ZeroVector)
		, Delta(FVector::ZeroVector)
		, bPress(false)
	{
	};
};

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTouchBeginEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTouchBeginEvent, FVector, Location);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTouchEndEvent, bool, bAllRelease);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTouchComponentMoveEvent, float, HorizontalDelta, float, VerticalDelta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTouchComponentMoveEvent, FVector, Delta, FVector, Loaction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTouchComponentPinchEvent, float, InAmount);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPT_API UTPTTouchInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTPTTouchInputComponent();

	virtual void BeginPlay() override;

	virtual void TouchBegin(const ETouchIndex::Type FingerIndex, FVector Location);
	virtual void TouchMove(const ETouchIndex::Type FingerIndex, FVector Location);

	void SetTouchDelta(const ETouchIndex::Type FingerIndex, const FVector& Location);

	void SetTouchInputRotDelta(float InTouchInputRotDelta) { TouchInputRotDelta = InTouchInputRotDelta; }
	void SetTouchInputPinchDelta(float InTouchInputPinchDelta) { TouchInputPinchDelta = InTouchInputPinchDelta; }

	virtual void TouchEnd(const ETouchIndex::Type FingerIndex, FVector Location);

	bool CheckMultiTouch();

private:
	ETPTTouchType::Type		TouchType;

	UPROPERTY(Transient)
		FTouchDataStruct	TouchData[ETouchIndex::Type::Touch10];

	UPROPERTY(Transient)
		float LastPinchLength;

	UPROPERTY(Transient)
		float LastYaw;

	UPROPERTY(Transient)
		float TouchInputRotDelta;

	UPROPERTY(Transient)
		float TouchInputPinchDelta;
public:
	UPROPERTY(BlueprintAssignable)
		FOnTouchComponentMoveEvent OnTouchMoveEvent;

	UPROPERTY(BlueprintAssignable)
		FOnTouchComponentPinchEvent OnTouchPinchEvent;

	UPROPERTY(BlueprintAssignable)
		FOnTouchBeginEvent OnTouchBeginEvent;

	UPROPERTY(BlueprintAssignable)
		FOnTouchEndEvent OnTouchEndEvent;
};
