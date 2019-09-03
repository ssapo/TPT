// Fill out your copyright notice in the Description page of Project Settings.

#include "TPTFullscreenWidget.h"
#include "Button.h"
#include "TextBlock.h"
#include "TPT.h"

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
		JumpButton->OnClicked.AddDynamic(this,
			&UTPTFullscreenWidget::JumpButtonClickedImpl);

		JumpButton->OnPressed.AddDynamic(this,
			&UTPTFullscreenWidget::JumpButtonPressedImpl);

		JumpButton->OnReleased.AddDynamic(this,
			&UTPTFullscreenWidget::JumpButtonReleasedImpl);

		/*JumpButton->OnHovered.AddDynamic(this,
			&UTPTFullscreenWidget::JumpButtonHoveredImpl);

		JumpButton->OnUnhovered.AddDynamic(this,
			&UTPTFullscreenWidget::JumpButtonUnHoveredImpl);*/
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
		JumpButton->OnPressed.RemoveAll(this);
		JumpButton->OnClicked.RemoveAll(this);
		JumpButton->OnReleased.RemoveAll(this);
		//JumpButton->OnHovered.RemoveAll(this);
		//JumpButton->OnUnhovered.RemoveAll(this);
	}
}

void UTPTFullscreenWidget::OnChangeLevelClikedImpl()
{
	const auto& GameMode = GetGameMode();
	GameMode.ChangeLevel(LevelName);
}

void UTPTFullscreenWidget::JumpButtonPressedImpl()
{
	TPT_PRINT_C(FColor::Cyan, TEXT("OnTouchTestButton1PressedImpl()"));
	const auto& GameMode = GetGameMode();
	GameMode.PCJump();
}

void UTPTFullscreenWidget::JumpButtonReleasedImpl()
{
	TPT_PRINT_C(FColor::Cyan, TEXT("OnTouchTestButton1ReleasedImpl()"));
	const auto& GameMode = GetGameMode();
	GameMode.PCStopJumping();
}

void UTPTFullscreenWidget::JumpButtonClickedImpl()
{
	TPT_PRINT_C(FColor::Cyan, TEXT("OnTouchTestButton1ClickedImpl()"));
}

void UTPTFullscreenWidget::JumpButtonHoveredImpl()
{
	TPT_PRINT_C(FColor::Cyan, TEXT("OnTouchTestButton1HoveredImpl()"));
}

void UTPTFullscreenWidget::JumpButtonUnHoveredImpl()
{
	TPT_PRINT_C(FColor::Cyan, TEXT("OnTouchTestButton1UnHoveredImpl()"));
}