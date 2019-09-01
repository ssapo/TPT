// Fill out your copyright notice in the Description page of Project Settings.

#include "TPTFullscreenWidget.h"
#include "Button.h"

void UTPTFullscreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (OnChangeLevelButton)
	{
		OnChangeLevelButton->OnClicked.AddDynamic(this,
			&UTPTFullscreenWidget::OnChangeLevelClikedImpl);
	}
}

void UTPTFullscreenWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (OnChangeLevelButton)
	{
		OnChangeLevelButton->OnClicked.RemoveAll(this);
	}
}

void UTPTFullscreenWidget::OnChangeLevelClikedImpl()
{
	const auto& GameMode = GetGameMode();
	GameMode.ChangeLevel(LevelName);
}
