// Copyright Epic Games, Inc. All Rights Reserved.


#include "MookSabalBattleGameModeBase.h"

#include "Character/LocalPlayerController.h"
#include "MookSabalBattle.h"
#include "MSBGameInstance.h"
#include "MSBGameStateBase.h"
#include "Character/PlayerCharacter.h"
#include "SessionManip/NullSessionSubsystem.h"

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
		if(it->IsA(ARedTeamPlayerStart::StaticClass()))
		{
			auto RedTeamStart = Cast<ARedTeamPlayerStart>(*it);
			FreeRedTeamPlayerStarts.Add(RedTeamStart);
		}
		else if(it->IsA(ABlueTeamPlayerStart::StaticClass()))
		{
			auto BlueTeamStart = Cast<ABlueTeamPlayerStart>(*it);
			FreeBlueTeamPlayerStarts.Add(BlueTeamStart);
		}
	}
	RepFinishedPlayerCount = 0;
	ApprovedPlayerNum = 0;

#if WITH_EDITOR
	GetGameInstance()->GetSubsystem<UNullSessionSubsystem>()->MaxPlayer = 2;
#endif
}

void AMookSabalBattleGameModeBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	// Join 요청 동시성 관리 : 남은 자리가 1일 때 두명이 거의 동시에 참가 요청을 보낸 경우
	CS_Login.Lock();
	if(ApprovedPlayerNum <= GetGameInstance()->GetSubsystem<UNullSessionSubsystem>()->MaxPlayer)
	{
		ErrorMessage = GameSession->ApproveLogin(Options);
		ApprovedPlayerNum++;
	}
	else
	{
		ErrorMessage = TEXT("Session is already full");
	}
	CS_Login.Unlock();

	FGameModeEvents::GameModePreLoginEvent.Broadcast(this, UniqueId, ErrorMessage);
}

FString AMookSabalBattleGameModeBase::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	TArray<FString> OptionsArray;
	Options.ParseIntoArray(OptionsArray, TEXT("?"));
	
	static int flag = 0;
	if(flag & 1)//홀수
	{
		auto FreeStart = FreeRedTeamPlayerStarts.Pop();
		NewPlayerController->StartSpot = FreeStart;
		PlayerStartMap.Add(NewPlayerController, FreeStart);
	}
	else
	{
		auto FreeStart = FreeBlueTeamPlayerStarts.Pop();
		NewPlayerController->StartSpot = FreeStart;
		PlayerStartMap.Add(NewPlayerController, FreeStart);
	}
	flag++;
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
	if(ControllerToNickname.Num() == GetGameInstance()->GetSubsystem<UNullSessionSubsystem>()->MaxPlayer)
		GetGameInstance()->GetSubsystem<UNullSessionSubsystem>()->StartSession();
	Super::PostLogin(NewPlayer);
}

void AMookSabalBattleGameModeBase::InitAllPlayers()
{
	for (auto PlayerController : PlayerControllers)
	{
		auto Character = Cast<APlayerCharacter>(PlayerController->GetPawn());
		FString Nickname = *ControllerToNickname.Find(PlayerController);
		
		UE_LOG(MookSablBattle, Log, TEXT("닉네임 ? %s"), *Nickname);
		PlayerController->GetPlayerState<ACharacterState>()->SetPlayerName(Nickname);
		if(PlayerController->StartSpot->IsA(ARedTeamPlayerStart::StaticClass()))
			Character->InitPlayer(true);
		else
			Character->InitPlayer(false);
	}

}

void AMookSabalBattleGameModeBase::IncreaseRepFinishedPlayerCount()
{
	RepFinishedPlayerCount++;
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("current finished player %d and max %d"), RepFinishedPlayerCount, GetGameInstance()->GetSubsystem<UNullSessionSubsystem>()->MaxPlayer));
	if(RepFinishedPlayerCount == GetGameInstance()->GetSubsystem<UNullSessionSubsystem>()->MaxPlayer) OnAllPlayerReplicationFinished.Execute();
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
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &AMookSabalBattleGameModeBase::ReturnToMainMenuHost);
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, DelayInSeconds, false);
}

void AMookSabalBattleGameModeBase::ReturnToMainMenuHost()
{
	GetGameInstance()->GetSubsystem<UNullSessionSubsystem>()->ExitSession();
	Super::ReturnToMainMenuHost();
}

