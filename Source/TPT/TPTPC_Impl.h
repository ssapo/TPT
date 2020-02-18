// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPTPC.h"
#include "TPTTouchInputComponent.h"
#include "SharedPointer.h"
#include "TPTPC_Impl.generated.h"

class UTPTTouchInputComponent;
class STPTVirtualJoystick;
class UTouchInterface;

UCLASS()
class TPT_API ATPTPC_Impl : public ATPTPC
{
	GENERATED_BODY()

public:
	ATPTPC_Impl();

	virtual void PostInitializeComponents() override;

	virtual void SetupInputComponent() override;

	virtual void PlayerTick(float DeltaTime) override;

	virtual void SetVirtualJoystickVisibility(bool bVisible) override;

	virtual void CreateTouchInterface() override;

	UFUNCTION(BlueprintPure)
		UTPTTouchInputComponent* GetTouchInputComponent() const;

	UFUNCTION()
		void OnLeftControllerTouchStarted();

	UFUNCTION()
		void OnLeftControllerTouched(const FVector2D& InThumbLocation, const FVector2D& InThumbMovement);

	UFUNCTION()
		void OnLeftControllerTouchedOver();

	void MoveForward(float Value);
	void MoveRight(float Value);

	void TurnAtRate(float Value);
	void LookUpRate(float Value);

	void SetBindAxis(bool bVisiblility = true);
	FRotator GetViewRotation() const;

	/** Ignore input value for Joy-pad(0 ~ 100%) */
	void SetTurnErrorIgnoreFactor(const int32 InFactor);

	void SetTouchInputRotDelta(float InTouchInputRotDelta);
	void SetTouchInputPinchDelta(float InTouchInputPinchDelta);

	void ReSetInputDir();

	void UpdateMovementInput(float DeltaTime);

protected:
	virtual void ActivateTouchInterface(UTouchInterface* NewTouchInterface) override;

	void ApplyMovement(const FVector& InMoveDirection, const float MoveAlpha);

protected:
	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName TurnAtRateBinding;
	static const FName LookUpRateBinding;

protected:
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
		UTPTTouchInputComponent* TouchInputComponent;

	TSharedPtr<STPTVirtualJoystick> SP_TPTVirtualJoystick;

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
