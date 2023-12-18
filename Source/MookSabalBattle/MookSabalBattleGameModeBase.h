// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MookSabalBattle.h"
#include "GameFramework/GameModeBase.h"
#include "MookSabalBattleGameModeBase.generated.h"

DECLARE_DELEGATE(FOnAllPlayerReplicationFinished);
/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API AMookSabalBattleGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMookSabalBattleGameModeBase();
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void PostInitializeComponents() override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	UPROPERTY()
	TArray<APlayerController*> PlayerControllers;
	TArray<APlayerStart*> FreePlayerStarts;
	TMap<AController*, APlayerStart*> PlayerStartMap;

	UFUNCTION()
	void InitAllPlayers();

	uint8 RepFinishedPlayerCount;

	FOnAllPlayerReplicationFinished OnAllPlayerReplicationFinished;

public:
	UFUNCTION()
	void IncreaseRepFinishedPlayerCount();

	UFUNCTION()
	void EndGamePlay();
};
