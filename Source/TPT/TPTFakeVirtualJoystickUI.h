
#pragma once

#include "CoreMinimal.h"
#include "TFTWidget.h"
#include "TPTFakeVirtualJoystickUI.generated.h"

class UCanvasPanelSlot;
class USizeBox;
class UCanvasPanel;
class UImage;
class USizeBox;
class UNamedSlot;

UCLASS()
class UTPTFakeVirtualJoystickUI : public UTFTWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;

private:
	UPROPERTY(meta = (BindWidgetOptional))
		UNamedSlot* FollowerSlot;

	UPROPERTY(meta = (BindWidget))
		USizeBox* SB_Interaction;

	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* CP_Joystick;

	UPROPERTY(meta = (BindWidget))
		UImage* ControllerImage;

	UPROPERTY(meta = (BindWidget))
		UImage* ThumbImage;
};
