// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MookSabalBattle.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Online.h"
#include "MSBGameInstance.generated.h"

DECLARE_DELEGATE_TwoParams(FOnSessionSearchCompleteWithResults, TArray<FOnlineSessionSearchResult>& SessionSearchResults, bool bSucceed)

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API UMSBGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;

	// Hosting Session
	UFUNCTION()
	void HostGame(FString NickName, int32 MaxPlayerCount, bool bUseLan);

	// Searching Session
	void TryFindSession(bool bUseLan);
	
	UFUNCTION()
	void OnFindSessionComplete(bool bSucceed);
	FOnSessionSearchCompleteWithResults OnSessionSearchCompleteWithResults;
	
	UFUNCTION()
	void EnterGameOnClient(FString ServerIP);
private:
	UFUNCTION()
	void EnterGame();
	
	IOnlineSubsystem* OnlineSubsystem;
	IOnlineSession* SessionInterface;
	TSharedPtr<FOnlineSessionSearch> LastSearchResult;

	// host
	UFUNCTION()
	void OnSessionCreate(FName SessionName, bool bWasSucceed);

	FString DessertMap;

	// A method retrying to get access to OnlineSubsystem and SessionInterface
	void GetSubsystemAndSessionInterface();
};
