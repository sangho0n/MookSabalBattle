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
	void EnterGameOnServer(FString ServerIP);
	UFUNCTION()
	void EnterGameOnClient(FString ServerIP);
private:
	UFUNCTION()
	void EnterGame();
	
	static IOnlineSubsystem* OnlineSubsystem;
	
	TSharedPtr<FOnlineSessionSettings> SessionSettings;
	
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	FString DessertMap;
};
