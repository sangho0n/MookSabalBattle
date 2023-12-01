// Copyright Epic Games, Inc. All Rights Reserved.


#include "MookSabalBattleGameModeBase.h"

#include "Character/LocalPlayerController.h"
#include "MookSabalBattle.h"
#include "MSBGameInstance.h"
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

void AMookSabalBattleGameModeBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	OnAllPlayerReplicationFinished.BindUObject(this, &AMookSabalBattleGameModeBase::InitAllPlayers);
}


void AMookSabalBattleGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	for(TActorIterator<APlayerStart> it(GetWorld()); it; ++it)
	{
		FreePlayerStarts.Add(*it);
	}
	RepFinishedPlayerCount = 0;
	MaxPlayerCount = Cast<UMSBGameInstance>(GetGameInstance())->MaxPlayer;
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

	NewPlayerController->StartSpot = FreePlayerStarts.Pop();
	PlayerControllers.Add(NewPlayerController);
	
	MSB_LOG(Log, TEXT("how many init new player called"));
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void AMookSabalBattleGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	MSB_LOG(Log, TEXT("how many post login called"));
	Super::PostLogin(NewPlayer);
}

void AMookSabalBattleGameModeBase::InitAllPlayers()
{
	MSB_LOG(Warning, TEXT("total controller : %d"), PlayerControllers.Num());

	int flag = 0;
	for(auto Controller : PlayerControllers)
	{
		auto Character = Cast<APlayerCharacter>(Controller->GetPawn());
		Character->InitPlayer(TEXT("default name"), flag%2==0?true:false);
		flag++;
	}

	Super::StartPlay();
}

void AMookSabalBattleGameModeBase::IncreaseRepFinishedPlayerCount()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("current finished player : %d"), RepFinishedPlayerCount));
	
	RepFinishedPlayerCount++;

	if(RepFinishedPlayerCount == MaxPlayerCount) OnAllPlayerReplicationFinished.Execute();
}
