// Copyright Epic Games, Inc. All Rights Reserved.


#include "MookSabalBattleGameModeBase.h"

#include "LocalPlayerController.h"
#include "MookSabalBattle.h"
#include "PlayerCharacter.h"

AMookSabalBattleGameModeBase::AMookSabalBattleGameModeBase()
{
	MSB_LOG_LOCATION(Warning);
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = ALocalPlayerController::StaticClass();
	MSB_LOG_LOCATION(Warning);
}
