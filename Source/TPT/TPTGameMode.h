// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPTGameMode.generated.h"

UCLASS(minimalapi)
class ATPTGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATPTGameMode();

	virtual void StartPlay() override;

	void ChangeLevel(const FName& InLevelName) const;

	void PCJump() const;

	void PCStopJumping() const;

private:
	UPROPERTY()
		TWeakObjectPtr<class ATPTPC> PCPtr;

	UPROPERTY()
		TWeakObjectPtr<class ATFTHUD> HUDPtr;
};



