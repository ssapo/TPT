// Fill out your copyright notice in the Description page of Project Settings.


#include "TFTWidget.h"
#include "TPT.h"

void UTFTWidget::NativeConstruct()
{
	Super::NativeConstruct();

	auto FoundGameMode = GetWorld()->GetAuthGameMode<ATPTGameMode>();
	TPT_CHECK(nullptr != FoundGameMode);

	TPTGameMode = FoundGameMode;

	bActivated = true;
}

void UTFTWidget::NativeDestruct()
{
	Super::NativeDestruct();

	bActivated = false;
}

bool UTFTWidget::IsActivating() const
{
	return bActivated;
}

const ATPTGameMode& UTFTWidget::GetGameMode() const
{
	return (*TPTGameMode);
}
