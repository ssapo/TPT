// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPTTouchInputComponent.h"
#include "TFTPC.generated.h"

UCLASS()
class TPT_API ATFTPC : public APlayerController
{
	GENERATED_BODY()

public:
	ATFTPC();

	virtual void PostInitializeComponents() override;

	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

	virtual void PlayerTick(float DeltaTime) override;

	virtual void SetPawn(APawn* InPawn) override;


public:
	UFUNCTION(BlueprintPure)
		class UTPTTouchInputComponent* GetTouchInputComponent() const;

	UFUNCTION()
		void OnLeftControllerTouchStarted();
	
	UFUNCTION()
		void OnLeftControllerTouched(const FVector2D& InThumbLocation, const FVector2D& InThumbMovement);

	UFUNCTION()
		void OnLeftControllerTouchedOver();

private:
	UFUNCTION()
		void CallBack_TouchBeginEvent(FVector InLocation);

	UFUNCTION()
		void CallBack_TouchEndEvent(bool InAllTouchesAreOver);

	UFUNCTION()
		void CallBack_TouchMoveEvent(FVector InDelta, FVector InLocation);

	UFUNCTION()
		void CallBack_TouchPinchEvent(float InAmount);

public:
	void MoveForward(float Value);
	void MoveRight(float Value);

	void TurnAtRate(float Value);
	void LookUpRate(float Value);

	void SetBindAxis(bool bVirtualkeyEnable);
	FRotator GetViewRotation() const;

	/** Ignore input value for Joy-pad(0 ~ 100%) */
	void SetTurnErrorIgnoreFactor(const int32 InFactor);

	void SetTouchInputRotDelta(float InTouchInputRotDelta);
	void SetTouchInputPinchDelta(float InTouchInputPinchDelta);

	void ReSetInputDir();

	void UpdateMovementInput(float DeltaTime);

protected:
	void ApplyMovement(const FVector& InMoveDirection, const float MoveAlpha);

protected:
	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName TurnAtRateBinding;
	static const FName LookUpRateBinding;

protected:
	UPROPERTY(Transient, BlueprintReadOnly)
		TWeakObjectPtr<class ATPTCharacter> CachedPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;
private:
#if WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING
	UPROPERTY(Transient)
		bool DirectInputControl;
#endif // WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING

	UPROPERTY(VisibleAnywhere)
		class UTPTTouchInputComponent* TouchInputComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (ClampMin = "20.0", ClampMax = "118.0", AllowPrivateAccess = true))
		float MaxLeftPadGradient;

	UPROPERTY(Transient)
		FVector InputDir;

	UPROPERTY(Transient)
		FVector InputDirFromController;

	UPROPERTY(Transient)
		float InputDirAlpha;

	UPROPERTY(Transient)
		FVector LastInputDir;

	UPROPERTY(Transient)
		FVector LastMovement;

	UPROPERTY(Transient)
		float TurnErrorIgnoreFactor;

	UPROPERTY(Transient)
		bool bLeftPad;
};
