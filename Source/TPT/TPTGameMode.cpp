// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TPTGameMode.h"
#include "TPTCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "TPT.h"
#include "TPTPC.h"
#include "TFTHUD.h"

ATPTGameMode::ATPTGameMode()
{
}

void ATPTGameMode::StartPlay()
{
	Super::StartPlay();

	auto PC = GetWorld()->GetFirstPlayerController<ATPTPC>();
	TPT_CHECK(nullptr != PC);
	PCPtr = PC;

	auto HUD = PCPtr->GetHUD();
	TPT_CHECK(nullptr != HUD);

	auto TouchTestHUD = Cast<ATFTHUD>(HUD);
	TPT_CHECK(nullptr != TouchTestHUD);
	HUDPtr = TouchTestHUD;
	HUDPtr->StartPlayHUD();
}

void ATPTGameMode::ChangeLevel(const FName& InLevelName) const
{
	TPT_PRINT_C(FColor::Red, TEXT("InLevelName: %s"), *InLevelName.ToString());
	UGameplayStatics::OpenLevel(this, InLevelName);
}

void ATPTGameMode::PCJump() const
{
	PCPtr->Jump();
}

void ATPTGameMode::PCStopJumping() const
{
	PCPtr->StopJumping();
}