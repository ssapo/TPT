// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTTouchZoneWIdget.h"
#include "TPT.h"
#include "Button.h"

void UTPTTouchZoneWIdget::NativeConstruct()
{
	Super::NativeConstruct();

	if (OnTouchTestButton1)
	{
		OnTouchTestButton1->OnClicked.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton1ClickedImpl);
		OnTouchTestButton1->OnPressed.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton1PressedImpl);
		OnTouchTestButton1->OnReleased.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton1ReleasedImpl);
		/*OnTouchTestButton1->OnHovered.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton1HoveredImpl);
		OnTouchTestButton1->OnUnhovered.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton1UnHoveredImpl);*/
	}

	if (OnTouchTestButton2)
	{
		OnTouchTestButton2->OnClicked.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton2ClickedImpl);
		OnTouchTestButton2->OnPressed.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton2PressedImpl);
		OnTouchTestButton2->OnReleased.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton2ReleasedImpl);
		/*	OnTouchTestButton2->OnHovered.AddDynamic(this,
				&UTPTTouchZoneWIdget::OnTouchTestButton2HoveredImpl);
			OnTouchTestButton2->OnUnhovered.AddDynamic(this,
				&UTPTTouchZoneWIdget::OnTouchTestButton2UnHoveredImpl);*/
	}

	if (OnTouchTestButton3)
	{
		OnTouchTestButton3->OnClicked.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton3ClickedImpl);
		OnTouchTestButton3->OnPressed.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton3PressedImpl);
		OnTouchTestButton3->OnReleased.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton3ReleasedImpl);
		/*OnTouchTestButton3->OnHovered.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton3HoveredImpl);
		OnTouchTestButton3->OnUnhovered.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton3UnHoveredImpl);*/
	}

	if (OnTouchTestButton4)
	{
		OnTouchTestButton4->OnClicked.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton4ClickedImpl);
		OnTouchTestButton4->OnPressed.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton4PressedImpl);
		OnTouchTestButton4->OnReleased.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton4ReleasedImpl);
		/*OnTouchTestButton4->OnHovered.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton4HoveredImpl);
		OnTouchTestButton4->OnUnhovered.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton4UnHoveredImpl);*/
	}
}

void UTPTTouchZoneWIdget::NativeDestruct()
{
	Super::NativeDestruct();

	if (OnTouchTestButton1)
	{
		OnTouchTestButton1->OnClicked.RemoveAll(this);
		OnTouchTestButton1->OnPressed.RemoveAll(this);
		OnTouchTestButton1->OnReleased.RemoveAll(this);
		//OnTouchTestButton1->OnHovered.RemoveAll(this);
		//OnTouchTestButton1->OnUnhovered.RemoveAll(this);
	}

	if (OnTouchTestButton2)
	{
		OnTouchTestButton2->OnClicked.RemoveAll(this);
		OnTouchTestButton2->OnPressed.RemoveAll(this);
		OnTouchTestButton2->OnReleased.RemoveAll(this);
		//OnTouchTestButton2->OnHovered.RemoveAll(this);
		//OnTouchTestButton2->OnUnhovered.RemoveAll(this);
	}

	if (OnTouchTestButton3)
	{
		OnTouchTestButton3->OnClicked.RemoveAll(this);
		OnTouchTestButton3->OnPressed.RemoveAll(this);
		OnTouchTestButton3->OnReleased.RemoveAll(this);
		//OnTouchTestButton3->OnHovered.RemoveAll(this);
		//OnTouchTestButton3->OnUnhovered.RemoveAll(this);
	}

	if (OnTouchTestButton4)
	{
		OnTouchTestButton4->OnClicked.RemoveAll(this);
		OnTouchTestButton4->OnPressed.RemoveAll(this);
		OnTouchTestButton4->OnReleased.RemoveAll(this);
		//OnTouchTestButton4->OnHovered.RemoveAll(this);
		//OnTouchTestButton4->OnUnhovered.RemoveAll(this);
	}
}


void UTPTTouchZoneWIdget::OnTouchTestButton1PressedImpl()
{
	TPT_PRINT_C(FColor::Red, TEXT("OnTouchTestButton1PressedImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton2PressedImpl()
{
	TPT_PRINT_C(FColor::Blue, TEXT("OnTouchTestButton2PressedImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton3PressedImpl()
{
	TPT_PRINT_C(FColor::Green, TEXT("OnTouchTestButton3PressedImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton4PressedImpl()
{
	TPT_PRINT_C(FColor::Yellow, TEXT("OnTouchTestButton4PressedImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton1ReleasedImpl()
{
	TPT_PRINT_C(FColor::Red, TEXT("OnTouchTestButton1ReleasedImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton2ReleasedImpl()
{
	TPT_PRINT_C(FColor::Blue, TEXT("OnTouchTestButton2ReleasedImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton3ReleasedImpl()
{
	TPT_PRINT_C(FColor::Green, TEXT("OnTouchTestButton3ReleasedImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton4ReleasedImpl()
{
	TPT_PRINT_C(FColor::Yellow, TEXT("OnTouchTestButton4ReleasedImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton1ClickedImpl()
{
	TPT_PRINT_C(FColor::Red, TEXT("OnTouchTestButton1ClickedImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton2ClickedImpl()
{
	TPT_PRINT_C(FColor::Blue, TEXT("OnTouchTestButton2ClickedImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton3ClickedImpl()
{
	TPT_PRINT_C(FColor::Green, TEXT("OnTouchTestButton3ClickedImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton4ClickedImpl()
{
	TPT_PRINT_C(FColor::Yellow, TEXT("OnTouchTestButton4ClickedImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton1HoveredImpl()
{
	TPT_PRINT_C(FColor::Red, TEXT("OnTouchTestButton1HoveredImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton2HoveredImpl()
{
	TPT_PRINT_C(FColor::Blue, TEXT("OnTouchTestButton2HoveredImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton3HoveredImpl()
{
	TPT_PRINT_C(FColor::Green, TEXT("OnTouchTestButton3HoveredImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton4HoveredImpl()
{
	TPT_PRINT_C(FColor::Yellow, TEXT("OnTouchTestButton4HoveredImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton1UnHoveredImpl()
{
	TPT_PRINT_C(FColor::Red, TEXT("OnTouchTestButton1UnHoveredImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton2UnHoveredImpl()
{
	TPT_PRINT_C(FColor::Blue, TEXT("OnTouchTestButton2UnHoveredImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton3UnHoveredImpl()
{
	TPT_PRINT_C(FColor::Green, TEXT("OnTouchTestButton3UnHoveredImpl()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton4UnHoveredImpl()
{
	TPT_PRINT_C(FColor::Yellow, TEXT("OnTouchTestButton4UnHoveredImpl()"));
}
