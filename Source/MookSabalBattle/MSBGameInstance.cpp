// Fill out your copyright notice in the Description page of Project Settings.


#include "MSBGameInstance.h"

void UMSBGameInstance::Init()
{
	Super::Init();
	OnlineSubsystem = IOnlineSubsystem::Get();
	SessionInterface = OnlineSubsystem->GetSessionInterface().Get();

	SessionInterface->OnCreateSessionCompleteDelegates.RemoveAll(this);
	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMSBGameInstance::OnSessionCreate);
	
	DessertMap = FString("/Game/Main/Maps/Map_Dessert");
}

void UMSBGameInstance::HostGame(FString NickName, int32 MaxPlayerCount, bool bUseLan)
{
	NickName = NickName.Append(TEXT("'s room"));
	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings.Get()->bIsDedicated = false;
	SessionSettings.Get()->NumPublicConnections = MaxPlayerCount;
	SessionSettings.Get()->bIsLANMatch = bUseLan;
	SessionInterface->CreateSession(MaxPlayerCount, FName(*NickName), *SessionSettings.Get());
}

void UMSBGameInstance::OnSessionCreate(FName SessionName, bool bWasSucceed)
{
	if(!bWasSucceed)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed to create session"));
		return;
	}
	MSB_LOG(Warning, TEXT("세션 생성 성공 ! %s"), *SessionName.ToString());
	GetWorld()->ServerTravel(DessertMap);
}


void UMSBGameInstance::EnterGame()
{
	MSB_LOG(Warning, TEXT("ddd"));
	GetWorld()->ServerTravel(DessertMap+TEXT("?listen"), true);

}


void UMSBGameInstance::EnterGameOnClient(FString ServerIP)
{

	auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->ClientTravel(ServerIP+TEXT(":7777"), TRAVEL_Absolute);
	MSB_LOG(Warning, TEXT("ddd"));
}
