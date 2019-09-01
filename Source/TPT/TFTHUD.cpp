// Fill out your copyright notice in the Description page of Project Settings.

#include "TFTHUD.h"
#include "TFTWidget.h"
#include "TPT.h"

void ATFTHUD::StartPlayHUD()
{
	HandlingWidgetImpl(StartPlayWidget);
}

void ATFTHUD::HandlingWidgetImpl(const TSubclassOf<UTFTWidget>& Widget)
{
	if (CurrentWidget.IsValid())
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}

	auto NewWidget = GetInactivatedWidget(Widget);
	if (!NewWidget)
	{
		NewWidget = CreateWidget<UTFTWidget>(GetWorld(), Widget);
		TPT_CHECK(nullptr != NewWidget);
	}

	NewWidget->AddToViewport();
	CurrentWidget = NewWidget;
}

UTFTWidget* ATFTHUD::GetInactivatedWidget(const TSubclassOf<UTFTWidget>& Key) const
{
	if (auto Found = WidgetPool.Find(Key))
	{
		if (false == (*Found)->IsActivating())
		{
			return *Found;
		}
	}

	return nullptr;
}
