// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"

DECLARE_LOG_CATEGORY_EXTERN(MookSablBattle, Log, All);

#define MSB_LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define MSB_LOG_LOCATION(Verbosity) UE_LOG(MookSablBattle, Verbosity, TEXT("%s"), *MSB_LOG_CALLINFO)
#define MSB_LOG(Verbosity, Format, ...) UE_LOG(MookSablBattle, Verbosity, TEXT("%s%s"), *MSB_LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))