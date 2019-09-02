// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTDPICustomScailingRule.h"

float UTPTDPICustomScailingRule::GetDPIScaleBasedOnSize(FIntPoint Size) const
{
	if (Size.X == 0 || Size.Y == 0)
		return 1.0f;

	// �����Ϲڽ��� EStretch::ScaleToFit �� ���� ������ ����� ���ش�.
	// �ٸ� �츮 ǥ�� 1280x768 �� ��������.. �ϴ� �� ���ڴ� �ϵ��ڵ�.
	const float StandardSizeX = 1280.0f;
	const float StandardSizeY = 768.0f;
	const float FinalScale = FMath::Min((float)Size.X / StandardSizeX, (float)Size.Y / StandardSizeY);

	return FinalScale;
}
