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
		void OnTouchTestButton1PressedImpl();

	UFUNCTION()
		void OnTouchTestButton2PressedImpl();

	UFUNCTION()
		void OnTouchTestButton3PressedImpl();

	UFUNCTION()
		void OnTouchTestButton4PressedImpl();

	UFUNCTION()
		void OnTouchTestButton1ReleasedImpl();

	UFUNCTION()
		void OnTouchTestButton2ReleasedImpl();

	UFUNCTION()
		void OnTouchTestButton3ReleasedImpl();

	UFUNCTION()
		void OnTouchTestButton4ReleasedImpl();

	UFUNCTION()
		void OnTouchTestButton1ClickedImpl();

	UFUNCTION()
		void OnTouchTestButton2ClickedImpl();

	UFUNCTION()
		void OnTouchTestButton3ClickedImpl();

	UFUNCTION()
		void OnTouchTestButton4ClickedImpl();

	UFUNCTION()
		void OnTouchTestButton1HoveredImpl();

	UFUNCTION()
		void OnTouchTestButton2HoveredImpl();

	UFUNCTION()
		void OnTouchTestButton3HoveredImpl();

	UFUNCTION()
		void OnTouchTestButton4HoveredImpl();

	UFUNCTION()
		void OnTouchTestButton1UnHoveredImpl();

	UFUNCTION()
		void OnTouchTestButton2UnHoveredImpl();

	UFUNCTION()
		void OnTouchTestButton3UnHoveredImpl();

	UFUNCTION()
		void OnTouchTestButton4UnHoveredImpl();


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
