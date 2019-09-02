// Fill out your copyright notice in the Description page of Project Settings.


#include "TFTPC.h"
//#include "Engine/Touch/LMTouchManager.h"
#include "TPTTouchInputComponent.h"
#include "TFTHUD.h"
#include "TPT.h"
#include "TPTCharacter.h"

const FName ATFTPC::MoveForwardBinding("MoveForward");
const FName ATFTPC::MoveRightBinding("MoveRight");
const FName ATFTPC::TurnAtRateBinding("TurnRate");
const FName ATFTPC::LookUpRateBinding("LookUpRate");

ATFTPC::ATFTPC()
{
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 55.f;
	InputDir = FVector::ZeroVector;
	InputDirAlpha = 0.f;

	TouchInputComponent = CreateDefaultSubobject<UTPTTouchInputComponent>("TouchInputComponent");

	bShowMouseCursor = true;

#if WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING
	DirectInputControl = false;
#endif // WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING
}

UTPTTouchInputComponent* ATFTPC::GetTouchInputComponent() const
{
	return TouchInputComponent;
}

void ATFTPC::OnLeftControllerTouchStarted()
{
	if (ATFTHUD* AsHUD = Cast<ATFTHUD>(GetHUD()))
	{
		AsHUD->OnLeftControllerTouchStarted();
	}
}

void ATFTPC::OnLeftControllerTouched(const FVector2D& InThumbLocation, const FVector2D& InThumbMovement)
{
	FVector2D ClampedDir = InThumbMovement;
	float SquaredSize = ClampedDir.SizeSquared();
	if (SquaredSize > 1.0f)
	{
		InputDirAlpha = FMath::Sqrt(SquaredSize) / MaxLeftPadGradient;
		ClampedDir.Normalize();
		bLeftPad = true;
	}

	InputDirFromController.X = -ClampedDir.Y;
	InputDirFromController.Y = ClampedDir.X;
}

void ATFTPC::OnLeftControllerTouchedOver()
{
	bLeftPad = false;
	InputDirAlpha = 0.f;

#if WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING
	DirectInputControl = false;
#endif // WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING

	ReSetInputDir();
}

void ATFTPC::MoveForward(float Value)
{
	InputDir.X = Value;
}

void ATFTPC::MoveRight(float Value)
{
	InputDir.Y = Value;
}

void ATFTPC::TurnAtRate(float Value)
{
	if (Value == 0.f)
	{
		return;
	}

	AddYawInput(Value * -BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATFTPC::LookUpRate(float Value)
{
	AddPitchInput(Value * -BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATFTPC::SetBindAxis(bool bVirtualkeyEnable)
{
	if (InputComponent == nullptr)
	{
		return;
	}

	InputComponent->AxisBindings.Empty();
	if (bVirtualkeyEnable)
	{
		InputComponent->BindAxis(MoveForwardBinding, this, &ATFTPC::MoveForward);
		InputComponent->BindAxis(MoveRightBinding, this, &ATFTPC::MoveRight);
		InputComponent->BindAxis(TurnAtRateBinding, this, &ATFTPC::TurnAtRate);
		InputComponent->BindAxis(LookUpRateBinding, this, &ATFTPC::LookUpRate);
	}
}

FRotator ATFTPC::GetViewRotation() const
{
	if (PlayerCameraManager != nullptr)
	{
		return PlayerCameraManager->GetCameraRotation();
	}
	else
	{
		FVector ViewLocation;
		FRotator ViewRotation;
		GetPlayerViewPoint(ViewLocation, ViewRotation);

		return ViewRotation;
	}
}

void ATFTPC::SetTurnErrorIgnoreFactor(const int32 InFactor)
{
	TurnErrorIgnoreFactor = FMath::Clamp(
		1.f - (InFactor * 0.01f),
		KINDA_SMALL_NUMBER,
		1.f - KINDA_SMALL_NUMBER);
}

void ATFTPC::ReSetInputDir()
{
	InputDir = FVector::ZeroVector;
	InputDirAlpha = 0.f;

#if WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING
	DirectInputControl = false;
#endif // WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING
}

void ATFTPC::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (IsMoveInputIgnored() == false)
	{
		UpdateMovementInput(DeltaTime);
	}
}

void ATFTPC::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (InPawn == nullptr)
	{
		return;
	}

	TPT_LOG(Log, TEXT("ATFTPC::SetPawn %s"), *InPawn->GetName());
	auto Character = Cast<ATPTCharacter>(InPawn);
	TPT_CHECK(Character != nullptr);
	CachedPawn = Character;
}

void ATFTPC::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SetTurnErrorIgnoreFactor(0);
}

void ATFTPC::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent == nullptr)
	{
		return;
	}

	//// Camera
	//InputComponent->BindAction("CameraZoomIn", IE_Pressed,
	//	this, &ATFTPC::CameraMouseZoomIn);

	//InputComponent->BindAction("CameraZoomOut", IE_Pressed,
	//	this, &ATFTPC::CameraMouseZoomOut);

	//// Movement
	//InputComponent->BindAction("SetDestination", IE_Pressed,
	//	this, &ATFTPC::OnSetDestinationPressed);

	//InputComponent->BindAction("SetDestination", IE_Released,
	//	this, &ATFTPC::OnSetDestinationReleased);

	// For joystick
	SetBindAxis(true);
}

void ATFTPC::UpdateMovementInput(float DeltaTime)
{
	if (InputDirFromController.SizeSquared2D() > KINDA_SMALL_NUMBER)
	{
		InputDir = InputDirFromController;
		InputDirFromController = FVector::ZeroVector;
	}

	if (InputDir.SizeSquared2D() <= KINDA_SMALL_NUMBER)
	{
		ApplyMovement(FVector::ZeroVector, 0.f);
		InputDirAlpha = 0.f;

#if WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING
		DirectInputControl = false;
#endif // WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING
		return;
	}

#if WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING
	if (DirectInputControl == true)
	{
		if (InputDirAlpha <= 0.f)
		{
			InputDirAlpha = 0.2f;
		}
		const float AccRate = 1.5f;
		InputDirAlpha += (DeltaTime * AccRate);
		InputDirAlpha = FMath::Min(InputDirAlpha, 1.f);
	}
#endif // WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING

	bool bUseTargetRotation = false;
	bool bUseViewRotation = false;
	const FVector NormalizedInputDir = InputDir.GetSafeNormal();

	if ((NormalizedInputDir | LastInputDir) < TurnErrorIgnoreFactor)
	{
		LastInputDir = NormalizedInputDir;
		bUseViewRotation = true;
	}
	else
	{
		bUseViewRotation = true;
	}

	// Camera Base
	if (bUseViewRotation == true)
	{
		FRotator ViewRotation = FRotator::ZeroRotator;
		ViewRotation.Yaw = GetViewRotation().Yaw;
		LastMovement = ViewRotation.RotateVector(LastInputDir);
	}

	TPT_LOG(Log, TEXT("%s %f"), *LastMovement.ToString(), InputDirAlpha);
	ApplyMovement(LastMovement, InputDirAlpha);
}

void ATFTPC::ApplyMovement(const FVector& InMoveDirection, const float MoveAlpha)
{
	if (CachedPawn.IsValid() == false)
	{
		return;
	}

	if (InMoveDirection.SizeSquared2D() > KINDA_SMALL_NUMBER)
	{
		CachedPawn->AddMovementInput(InMoveDirection, MoveAlpha);
	}
}


void ATFTPC::CallBack_TouchBeginEvent(FVector InLocation)
{

}

void ATFTPC::CallBack_TouchEndEvent(bool InAllTouchesAreOver)
{

}

void ATFTPC::CallBack_TouchMoveEvent(FVector InDelta, FVector InLocation)
{

}

void ATFTPC::CallBack_TouchPinchEvent(float InAmount)
{

}

void ATFTPC::BeginPlay()
{
	Super::BeginPlay();

	if (auto TIC = TouchInputComponent)
	{
		TIC->OnTouchBeginEvent.AddDynamic(this, &ATFTPC::CallBack_TouchBeginEvent);
		TIC->OnTouchEndEvent.AddDynamic(this, &ATFTPC::CallBack_TouchEndEvent);
		TIC->OnTouchMoveEvent.AddDynamic(this, &ATFTPC::CallBack_TouchMoveEvent);
		TIC->OnTouchPinchEvent.AddDynamic(this, &ATFTPC::CallBack_TouchPinchEvent);
	}
}

void ATFTPC::SetTouchInputRotDelta(float InTouchInputRotDelta)
{
	if (TouchInputComponent != nullptr)
	{
		TouchInputComponent->SetTouchInputRotDelta(InTouchInputRotDelta);
	}
}

void ATFTPC::SetTouchInputPinchDelta(float InTouchInputPinchDelta)
{
	if (TouchInputComponent != nullptr)
	{
		TouchInputComponent->SetTouchInputPinchDelta(InTouchInputPinchDelta);
	}
}