// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTDPICustomScailingRule.h"

float UTPTDPICustomScailingRule::GetDPIScaleBasedOnSize(FIntPoint Size) const
{
	if (Size.X == 0 || Size.Y == 0)
		return 1.0f;

	// 스케일박스의 EStretch::ScaleToFit 과 같은 스케일 계산을 해준다.
	// 다만 우리 표준 1280x768 을 기준으로.. 일단 이 숫자는 하드코딩.
	const float StandardSizeX = 1280.0f;
	const float StandardSizeY = 768.0f;
	const float FinalScale = FMath::Min((float)Size.X / StandardSizeX, (float)Size.Y / StandardSizeY);

	return FinalScale;
}
