// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"
#include "Engine/Engine.h"

DECLARE_LOG_CATEGORY_EXTERN(TPT, Log, All);

#define TPT_LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define TPT_LOG_S(Verbosity) UE_LOG(TPT, Verbosity, TEXT("%s"), *TPT_LOG_CALLINFO)
#define TPT_LOG(Verbosity, Format, ...) UE_LOG(TPT, Verbosity, TEXT("%s%s"), *TPT_LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

#define TPT_CHECK(Expr, ...) { if(!(Expr)) { TPT_LOG(Error, TEXT("%s -> ASSERTION : %s"), *TPT_LOG_CALLINFO, TEXT("'"#Expr"'")); return __VA_ARGS__; } }

#define TPT_PRINT_DETAIL(Key, Color, Sec, NewerOnTop, Format, ...) { if(GEngine) { GEngine->AddOnScreenDebugMessage(Key, Sec, Color, FString::Printf(Format, ##__VA_ARGS__), NewerOnTop, FVector2D::UnitVector); } }

#define FAST_PRINT(Color) TPT_PRINT_DETAIL(-1, Color, 3.0f, true, TEXT("%s"), *TPT_LOG_CALLINFO)

#define TPT_PRINT(Format, ...) TPT_PRINT_DETAIL(-1, FColor::Green, 3.0f, true,  Format, ##__VA_ARGS__)

#define TPT_PRINT_C(Color, Format, ...) TPT_PRINT_DETAIL(-1, Color, 3.0f, true, Format, ##__VA_ARGS__)

#define TPT_PRINT_CS(Color, Sec, Format, ...) TPT_PRINT_DETAIL(-1, Color, Sec, true, Format, ##__VA_ARGS__)

