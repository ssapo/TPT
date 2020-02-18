#include "TPTFakeVirtualJoystickUI.h"

#include "GameFramework/InputSettings.h"
#include "GameFramework/TouchInterface.h"
#include "Engine/Texture2D.h"
#include "CanvasPanelSlot.h"
#include "SizeBox.h"
#include "Engine/UserInterfaceSettings.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"

void UTPTFakeVirtualJoystickUI::NativePreConstruct()
{
	Super::NativePreConstruct();

	FSoftObjectPath DefaultTouchInterfaceName = GetDefault<UInputSettings>()->DefaultTouchInterface;
	if (DefaultTouchInterfaceName.IsValid() == false)
	{
		return;
	}

	if (UTouchInterface* CurrentTouchInterface = LoadObject<UTouchInterface>(NULL, *DefaultTouchInterfaceName.ToString()))
	{
		if (CurrentTouchInterface->Controls.Num() <= 0)
		{
			return;
		}

		FTouchInputControl Control = CurrentTouchInterface->Controls[0];

		if (UCanvasPanelSlot* CPSlot = Cast<UCanvasPanelSlot>(CP_Joystick->Slot))
		{
			CPSlot->SetPosition(Control.Center);
		}
		
		SB_Interaction->SetWidthOverride(Control.InteractionSize.X);
		SB_Interaction->SetHeightOverride(Control.InteractionSize.Y);
		ControllerImage->SetBrushFromTexture(Control.Image2);
		ControllerImage->SetBrushSize(Control.VisualSize);
		ThumbImage->SetBrushFromTexture(Control.Image1);
		ThumbImage->SetBrushSize(Control.ThumbSize);

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(this->Slot))
		{
			FVector2D Alignment = CanvasSlot->GetAlignment();
			FVector2D Size = Control.InteractionSize;
			FVector2D Position((Size.X * Alignment.X), -Size.Y + (Size.Y * Alignment.Y));

			// set force position
			CanvasSlot->SetPosition(Position);
		}
	}
}