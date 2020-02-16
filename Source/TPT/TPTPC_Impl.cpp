#include "TPTPC_Impl.h"
#include "TPTTouchInputComponent.h"
#include "TFTHUD.h"
#include "TPT.h"
#include "TPTCharacter.h"
#include "GameFramework/PlayerController.h"
#include "TPTVirtualJoystick.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/TouchInterface.h"
#include "TPTVirtualJoystick.h"
#include "SlateWrapperTypes.h"

const FName ATPTPC_Impl::MoveForwardBinding("MoveForward");
const FName ATPTPC_Impl::MoveRightBinding("MoveRight");
const FName ATPTPC_Impl::TurnAtRateBinding("TurnRate");
const FName ATPTPC_Impl::LookUpRateBinding("LookUpRate");

ATPTPC_Impl::ATPTPC_Impl()
{
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 55.f;
	InputDir = FVector::ZeroVector;
	InputDirAlpha = 0.f;

	TouchInputComponent = CreateDefaultSubobject<UTPTTouchInputComponent>("TouchInputComponent");

#if WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING
	DirectInputControl = false;
#endif // WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING
}

UTPTTouchInputComponent* ATPTPC_Impl::GetTouchInputComponent() const
{
	return TouchInputComponent;
}

void ATPTPC_Impl::OnLeftControllerTouchStarted()
{
	if (ATFTHUD* AsHUD = Cast<ATFTHUD>(GetHUD()))
	{
		AsHUD->OnLeftControllerTouchStarted();
	}
}

void ATPTPC_Impl::OnLeftControllerTouched(const FVector2D& InThumbLocation, const FVector2D& InThumbMovement)
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

void ATPTPC_Impl::OnLeftControllerTouchedOver()
{
	bLeftPad = false;
	InputDirAlpha = 0.f;

#if WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING
	DirectInputControl = false;
#endif // WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING

	ReSetInputDir();
}

void ATPTPC_Impl::MoveForward(float Value)
{
	InputDir.X = Value;
}

void ATPTPC_Impl::MoveRight(float Value)
{
	InputDir.Y = Value;
}

void ATPTPC_Impl::TurnAtRate(float Value)
{
	if (Value == 0.f)
	{
		return;
	}

	AddYawInput(Value * -BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATPTPC_Impl::LookUpRate(float Value)
{
	AddPitchInput(Value * -BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATPTPC_Impl::SetBindAxis(bool bVisiblility /*= true*/)
{
	if (InputComponent == nullptr)
	{
		return;
	}

	InputComponent->AxisBindings.Empty();
	if (bVisiblility)
	{
		InputComponent->BindAxis(MoveForwardBinding, this, &ATPTPC_Impl::MoveForward);
		InputComponent->BindAxis(MoveRightBinding, this, &ATPTPC_Impl::MoveRight);
		InputComponent->BindAxis(TurnAtRateBinding, this, &ATPTPC_Impl::TurnAtRate);
		InputComponent->BindAxis(LookUpRateBinding, this, &ATPTPC_Impl::LookUpRate);
	}
}

FRotator ATPTPC_Impl::GetViewRotation() const
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

void ATPTPC_Impl::SetTurnErrorIgnoreFactor(const int32 InFactor)
{
	TurnErrorIgnoreFactor = FMath::Clamp(
		1.f - (InFactor * 0.01f),
		KINDA_SMALL_NUMBER,
		1.f - KINDA_SMALL_NUMBER);
}

void ATPTPC_Impl::ReSetInputDir()
{
	InputDir = FVector::ZeroVector;
	InputDirAlpha = 0.f;

#if WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING
	DirectInputControl = false;
#endif // WITH_EDITORONLY_DATA && !UE_BUILD_SHIPPING
}

void ATPTPC_Impl::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (IsMoveInputIgnored() == false)
	{
		UpdateMovementInput(DeltaTime);
	}
}

void ATPTPC_Impl::SetVirtualJoystickVisibility(bool bVisible)
{
	Super::SetVirtualJoystickVisibility(bVisible);

	if (InputComponent == nullptr)
	{
		return;
	}

	SetBindAxis(bVisible);
}

void ATPTPC_Impl::CreateTouchInterface()
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);

	// do we want to show virtual joysticks?
	if (LocalPlayer && LocalPlayer->ViewportClient && SVirtualJoystick::ShouldDisplayTouchInterface())
	{
		// in case we already had one, remove it
		if (VirtualJoystick.IsValid())
		{
			Cast<ULocalPlayer>(Player)->ViewportClient->RemoveViewportWidgetContent(VirtualJoystick.ToSharedRef());
		}

		if (CurrentTouchInterface == nullptr)
		{
			// load what the game wants to show at startup
			FSoftObjectPath DefaultTouchInterfaceName = GetDefault<UInputSettings>()->DefaultTouchInterface;

			if (DefaultTouchInterfaceName.IsValid())
			{
				// activate this interface if we have it
				CurrentTouchInterface = LoadObject<UTouchInterface>(NULL, *DefaultTouchInterfaceName.ToString());
			}
		}

		if (CurrentTouchInterface)
		{
			// create the joystick 
			SP_TPTVirtualJoystick = SNew(STPTVirtualJoystick)
				.Owner(this)
				.OnMovementStart(BIND_UOBJECT_DELEGATE(FSimpleDelegate, OnLeftControllerTouchStarted))
				.OnMovementOver(BIND_UOBJECT_DELEGATE(FSimpleDelegate, OnLeftControllerTouchedOver))
				.OnThumbDeltaEvent(BIND_UOBJECT_DELEGATE(FThumbDeltaEvent, OnLeftControllerTouched));

			VirtualJoystick = SP_TPTVirtualJoystick;

			// add it to the player's viewport
			LocalPlayer->ViewportClient->AddViewportWidgetContent(VirtualJoystick.ToSharedRef());

			ActivateTouchInterface(CurrentTouchInterface);
		}
	}
}

void ATPTPC_Impl::ActivateTouchInterface(UTouchInterface* NewTouchInterface)
{
	CurrentTouchInterface = NewTouchInterface;
	if (NewTouchInterface)
	{
		if (!VirtualJoystick.IsValid())
		{
			CreateTouchInterface();
		}
		else
		{
			NewTouchInterface->Activate(VirtualJoystick);

			SP_TPTVirtualJoystick->CopyControls();
		}
	}
	else if (VirtualJoystick.IsValid())
	{
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
		if (LocalPlayer && LocalPlayer->ViewportClient)
		{
			LocalPlayer->ViewportClient->RemoveViewportWidgetContent(VirtualJoystick.ToSharedRef());
		}
		//clear any input before clearing the VirtualJoystick
		FlushPressedKeys();
		VirtualJoystick = NULL;
	}
}

void ATPTPC_Impl::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SetTurnErrorIgnoreFactor(0);
}

void ATPTPC_Impl::SetupInputComponent()
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
	SetBindAxis();
}

void ATPTPC_Impl::UpdateMovementInput(float DeltaTime)
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

	//TPT_PRINT(TEXT("InputDirAlpha: %f"), InputDirAlpha);
	ApplyMovement(LastMovement, InputDirAlpha);
}

void ATPTPC_Impl::ApplyMovement(const FVector& InMoveDirection, const float MoveAlpha)
{
	if (CachedPCPtr.IsValid() == false)
	{
		return;
	}

	if (InMoveDirection.SizeSquared2D() > KINDA_SMALL_NUMBER)
	{
		CachedPCPtr->AddMovementInput(InMoveDirection, MoveAlpha);
	}
}

void ATPTPC_Impl::SetTouchInputRotDelta(float InTouchInputRotDelta)
{
	if (TouchInputComponent != nullptr)
	{
		TouchInputComponent->SetTouchInputRotDelta(InTouchInputRotDelta);
	}
}

void ATPTPC_Impl::SetTouchInputPinchDelta(float InTouchInputPinchDelta)
{
	if (TouchInputComponent != nullptr)
	{
		TouchInputComponent->SetTouchInputPinchDelta(InTouchInputPinchDelta);
	}
}