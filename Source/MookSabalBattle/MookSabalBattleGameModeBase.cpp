// Copyright Epic Games, Inc. All Rights Reserved.


#include "MookSabalBattleGameModeBase.h"

#include "Character/LocalPlayerController.h"
#include "MookSabalBattle.h"
#include "Character/PlayerCharacter.h"

AMookSabalBattleGameModeBase::AMookSabalBattleGameModeBase()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = ALocalPlayerController::StaticClass();
}
