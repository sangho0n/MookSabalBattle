// Copyright Epic Games, Inc. All Rights Reserved.


#include "MookSabalBattleGameModeBase.h"

#include "Character/LocalPlayerController.h"
#include "MookSabalBattle.h"
#include "Character/PlayerCharacter.h"

AMookSabalBattleGameModeBase::AMookSabalBattleGameModeBase()
{
	ConstructorHelpers::FClassFinder<APlayerCharacter> BP_CHARACTER(TEXT("/Game/Main/Actors/BP_PlayerCharacter.BP_PlayerCharacter_C"));
	if(BP_CHARACTER.Succeeded())
	{
		DefaultPawnClass = BP_CHARACTER.Class;
	}
	PlayerControllerClass = ALocalPlayerController::StaticClass();
	PlayerStateClass = ACharacterState::StaticClass();
}

void AMookSabalBattleGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	for(TActorIterator<APlayerStart> it(GetWorld()); it; ++it)
	{
		FreePlayerStarts.Add(*it);
	}
}

void AMookSabalBattleGameModeBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	if(FreePlayerStarts.Num() == 0)
	{
		ErrorMessage = TEXT("Server Full");
	}

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

FString AMookSabalBattleGameModeBase::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	if(FreePlayerStarts.Num() == 0)
	{
		MSB_LOG(Error, TEXT("No free player starts"));
		return FString(TEXT("No free player starts"));
	}

	NewPlayerController->StartSpot = FreePlayerStarts.Pop();;
	PlayerControllers.Add(NewPlayerController);
	
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void AMookSabalBattleGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}
