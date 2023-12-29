// Copyright Epic Games, Inc. All Rights Reserved.


#include "MookSabalBattleGameModeBase.h"

#include "Character/LocalPlayerController.h"
#include "MookSabalBattle.h"
#include "MSBGameInstance.h"
#include "MSBGameStateBase.h"
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
	GameStateClass = AMSBGameStateBase::StaticClass();
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

	// TODO : 아래 코드는 PIE 용임.
	Cast<UMSBGameInstance>(GetGameInstance())->MaxPlayer = 2;
	PlayerStartMap.Reserve(Cast<UMSBGameInstance>(GetGameInstance())->MaxPlayer);
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
	// FString NickName = Cast<UMSBGameInstance>(GetGameInstance())->PlayerNickNames.Last();
	// auto MSBGameState = Cast<AMSBGameStateBase>(GameState);
	// auto PlayerState = Cast<ALocalPlayerController>(NewPlayerController)->GetPlayerState<ACharacterState>();

	auto FreeStart = FreePlayerStarts.Pop();
	NewPlayerController->StartSpot = FreeStart;
	PlayerStartMap.Add(NewPlayerController, FreeStart);
	PlayerControllers.Add(NewPlayerController);
	// PlayerState->NickName = NickName;
	// UE_LOG(MookSablBattle, Log, TEXT("nickname ? %s"), *PlayerState->NickName);
	
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void AMookSabalBattleGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	MSB_LOG(Log, TEXT("how many post login called"));
	Super::PostLogin(NewPlayer);
}

void AMookSabalBattleGameModeBase::InitAllPlayers()
{
	auto MSBGameState = Cast<AMSBGameStateBase>(GameState);

	int flag = 0;
	bool bFlag;
	for(auto Controller : PlayerControllers)
	{
		auto Character = Cast<APlayerCharacter>(Controller->GetPawn());
		// FString NickName = Character->GetCharacterStateComponent()->NickName;
		// UE_LOG(MookSablBattle, Log, TEXT("character %s"), *Character->GetName());
		// UE_LOG(MookSablBattle, Log, TEXT("is null ? %s"), *NickName);
		bFlag = flag%2==0?true:false;// TODO why read access violation?
		Character->InitPlayer(TEXT("default name"), bFlag);
		
		flag++;
	}

}

void AMookSabalBattleGameModeBase::IncreaseRepFinishedPlayerCount()
{
	RepFinishedPlayerCount++;
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("current finished player %d and max %d"), RepFinishedPlayerCount, Cast<UMSBGameInstance>(GetGameInstance())->MaxPlayer));
	if(RepFinishedPlayerCount == Cast<UMSBGameInstance>(GetGameInstance())->MaxPlayer) OnAllPlayerReplicationFinished.Execute();
}

AActor* AMookSabalBattleGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	return *PlayerStartMap.Find(Player);
}

void AMookSabalBattleGameModeBase::EndGamePlay()
{
	// freeze
	//for (APlayerController* PlayerController : PlayerControllers) 왜 이 루프로 순회하면 클라이언트 RPC가 호출이 안되는 것일까...
	for (TActorIterator<APlayerController> it(GetWorld()); it; ++it)
	{
		auto PlayerController = *it;
		Cast<ALocalPlayerController>(PlayerController)->DisableInputEachCient(PlayerController);
	}
}
