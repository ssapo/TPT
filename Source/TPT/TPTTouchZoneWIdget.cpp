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
			&UTPTTouchZoneWIdget::OnTouchTestButton1Clicked);
	}

	if (OnTouchTestButton2)
	{
		OnTouchTestButton2->OnClicked.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton2Clicked);
	}

	if (OnTouchTestButton3)
	{
		OnTouchTestButton3->OnClicked.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton3Clicked);
	}

	if (OnTouchTestButton4)
	{
		OnTouchTestButton4->OnClicked.AddDynamic(this,
			&UTPTTouchZoneWIdget::OnTouchTestButton4Clicked);
	}
}

void UTPTTouchZoneWIdget::NativeDestruct()
{
	Super::NativeDestruct();

	if (OnTouchTestButton1)
	{
		OnTouchTestButton1->OnClicked.RemoveAll(this);
	}

	if (OnTouchTestButton2)
	{
		OnTouchTestButton2->OnClicked.RemoveAll(this);
	}

	if (OnTouchTestButton3)
	{
		OnTouchTestButton3->OnClicked.RemoveAll(this);
	}

	if (OnTouchTestButton4)
	{
		OnTouchTestButton4->OnClicked.RemoveAll(this);
	}
}


void UTPTTouchZoneWIdget::OnTouchTestButton1Clicked()
{
	TPT_PRINT_C(FColor::Red, TEXT("OnTouchTestButton1Clicked()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton2Clicked()
{
	TPT_PRINT_C(FColor::Blue, TEXT("OnTouchTestButton2Clicked()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton3Clicked()
{
	TPT_PRINT_C(FColor::Green, TEXT("OnTouchTestButton3Clicked()"));
}

void UTPTTouchZoneWIdget::OnTouchTestButton4Clicked()
{
	TPT_PRINT_C(FColor::Yellow, TEXT("OnTouchTestButton4Clicked()"));
}
