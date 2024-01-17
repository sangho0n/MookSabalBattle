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
	
	ControllerToNickname.Empty();

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

	TArray<FString> OptionsArray;
	Options.ParseIntoArray(OptionsArray, TEXT("?"));
	
	auto FreeStart = FreePlayerStarts.Pop();
	NewPlayerController->StartSpot = FreeStart;
	PlayerStartMap.Add(NewPlayerController, FreeStart);
	PlayerControllers.Add(NewPlayerController);

	for(const auto& Option : OptionsArray)
	{
		if(Option.StartsWith(TEXT("Nickname")))
		{
			FString Nickname;
			FParse::Value(*Option, TEXT("Nickname="), Nickname);
			//NewPlayerController->GetPlayerState<ACharacterState>()->SetPlayerName(Nickname);
			ControllerToNickname.Add(NewPlayerController, Nickname);
			UE_LOG(MookSablBattle, Log, TEXT("닉네임 변경 시도 %s"), *Nickname);
			break;
		}
		
		//NewPlayerController->GetPlayerState<ACharacterState>()->SetPlayerName(TEXT("default"));
		ControllerToNickname.Add(NewPlayerController, TEXT("default"));
	}
	
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void AMookSabalBattleGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	MSB_LOG(Log, TEXT("how many post login called"));
	Super::PostLogin(NewPlayer);
}

void AMookSabalBattleGameModeBase::InitAllPlayers()
{
	int flag = 0;
	bool bFlag;
	for (auto PlayerController : PlayerControllers)
	{
		auto Character = Cast<APlayerCharacter>(PlayerController->GetPawn());
		bFlag = flag%2==0?true:false;
		FString Nickname = *ControllerToNickname.Find(PlayerController);
		
		UE_LOG(MookSablBattle, Log, TEXT("닉네임 ? %s"), *Nickname);
		PlayerController->GetPlayerState<ACharacterState>()->SetPlayerName(Nickname);
		Character->InitPlayer(bFlag);
		
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
	for (TActorIterator<APlayerController> it(GetWorld()); it; ++it)
	{
		auto PlayerController = *it;
		Cast<ALocalPlayerController>(PlayerController)->DisableInputEachCient(PlayerController);
	}
	
	FTimerHandle TimerHandle;
	float DelayInSeconds = 5.0f;
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &AGameModeBase::ReturnToMainMenuHost);
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, DelayInSeconds, false);
}
