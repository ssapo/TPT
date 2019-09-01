// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TPTGameMode.h"
#include "TFTWidget.generated.h"

UCLASS()
class TPT_API UTFTWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintPure)
		bool IsActivating() const;

	const ATPTGameMode& GetGameMode() const;

private:
	bool bActivated;

	ATPTGameMode* TPTGameMode;
};
