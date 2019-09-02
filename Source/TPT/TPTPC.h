// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPTPC.generated.h"

/**
 * 
 */
UCLASS()
class TPT_API ATPTPC : public APlayerController
{
	GENERATED_BODY()
	
public:
	ATPTPC();

	virtual void SetPawn(APawn* InPawn) override;

	void Jump();
	void StopJumping();

protected:
	UPROPERTY(Transient, BlueprintReadOnly)
		TWeakObjectPtr<class ATPTCharacter> CachedPCPtr;

};
