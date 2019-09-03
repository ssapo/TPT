// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTPC.h"
#include "TPT.h"
#include "TPTCharacter.h"
#include "SlateApplication.h"

ATPTPC::ATPTPC()
{
	bShowMouseCursor = true;
}

void ATPTPC::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (InPawn == nullptr)
	{
		return;
	}

	TPT_LOG(Log, TEXT("ATPTPC::SetPawn %s"), *InPawn->GetName());
	auto Character = Cast<ATPTCharacter>(InPawn);
	TPT_CHECK(Character != nullptr);
	CachedPCPtr = Character;
}

void ATPTPC::Jump()
{
	if (CachedPCPtr.IsValid())
	{
		CachedPCPtr->Jump();
	}
}

void ATPTPC::StopJumping()
{
	if (CachedPCPtr.IsValid())
	{
		CachedPCPtr->StopJumping();
	}
}