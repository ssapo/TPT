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

	UFUNCTION()
		void JumpButtonPressedImpl();

	UFUNCTION()
		void JumpButtonReleasedImpl();

protected:
	virtual void NativePreConstruct() override;

private:
	UPROPERTY(Meta = (BindWidget))
		class UButton* ChangeLevelButton;

	UPROPERTY(Meta = (BindWidget))
		class UButton* JumpButton;

	UPROPERTY(Meta = (BindWidget))
		class UTextBlock* ChangeButtonText;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		FName LevelName;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		FText ButtonText;
};
