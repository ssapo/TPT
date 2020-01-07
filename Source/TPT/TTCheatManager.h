#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "TTCheatManager.generated.h"

UCLASS()
class TPT_API UTTCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
private:
	UFUNCTION(exec)
		void DrawJoystickArea();
};
