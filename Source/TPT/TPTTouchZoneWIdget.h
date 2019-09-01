// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TFTWidget.h"
#include "TPTTouchZoneWIdget.generated.h"

/**
 * 
 */
UCLASS()
class TPT_API UTPTTouchZoneWIdget : public UTFTWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
		void OnTouchTestButton1Clicked();

	UFUNCTION()
		void OnTouchTestButton2Clicked();

	UFUNCTION()
		void OnTouchTestButton3Clicked();

	UFUNCTION()
		void OnTouchTestButton4Clicked();

private:
	UPROPERTY(Meta = (BindWidget))
		class UButton* OnTouchTestButton1;

	UPROPERTY(Meta = (BindWidget))
		class UButton* OnTouchTestButton2;

	UPROPERTY(Meta = (BindWidget))
		class UButton* OnTouchTestButton3;

	UPROPERTY(Meta = (BindWidget))
		class UButton* OnTouchTestButton4;
};
