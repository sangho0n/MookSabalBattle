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
	// TODO MaxPlayer를 호스팅할 때 동적으로 변경 및 패키징된 파일에서는 왜 MaxPlayerCount가 채워지기도 전에 OnAllPlayerReplicationFinished가 broadcast되는지 디버깅하기
	MaxPlayerCount = 2;
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
	
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void AMookSabalBattleGameModeBase::PostLogin(APlayerController* NewPlayer)
{
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
}

void AMookSabalBattleGameModeBase::IncreaseRepFinishedPlayerCount()
{
	RepFinishedPlayerCount++;
	if(RepFinishedPlayerCount == MaxPlayerCount) OnAllPlayerReplicationFinished.Execute();
}
