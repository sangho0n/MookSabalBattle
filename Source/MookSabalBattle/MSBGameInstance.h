// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MookSabalBattle.h"
#include "OnlineSessionSettings.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "OnlineSubsystem.h"
#include "Online.h"
#include "OnlineSessionSettings.h"
#include "MSBGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API UMSBGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;
	UFUNCTION()
	void HostGame(FString NickName, int32 MaxPlayerCount, bool bUseLan);
	
	UFUNCTION()
	void EnterGameOnClient(FString ServerIP);
private:
	UFUNCTION()
	void EnterGame();
	
	IOnlineSubsystem* OnlineSubsystem;
	IOnlineSession* SessionInterface;

	// host
	UFUNCTION()
	void OnSessionCreate(FName SessionName, bool bWasSucceed);
	
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	FString DessertMap;
};
