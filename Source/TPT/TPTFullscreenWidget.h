// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TFTWidget.h"
#include "TPTFullscreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class TPT_API UTPTFullscreenWidget : public UTFTWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
		void OnChangeLevelClikedImpl();

private:
	UPROPERTY(Meta = (BindWidget))
		class UButton* OnChangeLevelButton;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		FName LevelName;

};
