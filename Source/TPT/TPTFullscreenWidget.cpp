// Fill out your copyright notice in the Description page of Project Settings.

#include "TPTFullscreenWidget.h"
#include "Button.h"
#include "TextBlock.h"

void UTPTFullscreenWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	ChangeButtonText->SetText(ButtonText);
}

void UTPTFullscreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ChangeLevelButton)
	{
		ChangeLevelButton->OnClicked.AddDynamic(this,
			&UTPTFullscreenWidget::OnChangeLevelClikedImpl);
	}

	if (JumpButton)
	{
		JumpButton->OnPressed.AddDynamic(this,
			&UTPTFullscreenWidget::JumpButtonPressedImpl);

		JumpButton->OnReleased.AddDynamic(this,
			&UTPTFullscreenWidget::JumpButtonReleasedImpl);
	}
}

void UTPTFullscreenWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (ChangeLevelButton)
	{
		ChangeLevelButton->OnClicked.RemoveAll(this);
	}

	if (JumpButton)
	{
		JumpButton->OnClicked.RemoveAll(this);
	}
}

void UTPTFullscreenWidget::OnChangeLevelClikedImpl()
{
	const auto& GameMode = GetGameMode();
	GameMode.ChangeLevel(LevelName);
}

void UTPTFullscreenWidget::JumpButtonPressedImpl()
{
	const auto& GameMode = GetGameMode();
	GameMode.PCJump();
}

void UTPTFullscreenWidget::JumpButtonReleasedImpl()
{
	const auto& GameMode = GetGameMode();
	GameMode.PCStopJumping();
}
