// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TFTHUD.generated.h"

class UTFTWidget;

UCLASS()
class TPT_API ATFTHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	void StartPlayHUD();

	UFUNCTION(BlueprintImplementableEvent)
		void OnLeftControllerTouchStarted_BP();

	void OnLeftControllerTouchStarted();

protected:
	void HandlingWidgetImpl(const TSubclassOf<UTFTWidget>& Widget);

	UTFTWidget* GetInactivatedWidget(const TSubclassOf<UTFTWidget>& Key) const;

protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		TSubclassOf<UTFTWidget> StartPlayWidget;

	TMap<TSubclassOf<UTFTWidget>, UTFTWidget*> WidgetPool;

	TWeakObjectPtr<UTFTWidget> CurrentWidget;
};
