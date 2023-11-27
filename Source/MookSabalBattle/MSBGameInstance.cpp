// Fill out your copyright notice in the Description page of Project Settings.


#include "MSBGameInstance.h"
#include "Online/OnlineSessionNames.h"

void UMSBGameInstance::Init()
{
	Super::Init();
	GetSubsystemAndSessionInterface();

	SessionInterface->OnCreateSessionCompleteDelegates.RemoveAll(this);
	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMSBGameInstance::OnSessionCreate);
	SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMSBGameInstance::OnFindSessionComplete);
	
	DessertMap = FString("/Game/Main/Maps/Map_Dessert");
}

void UMSBGameInstance::HostGame(FString NickName, int32 MaxPlayerCount, bool bUseLan)
{
	GetSubsystemAndSessionInterface();
	if(OnlineSubsystem == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get online subsystem"));
		return;
	}
	if(SessionInterface == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get session interface"));
		return;
	}

	FOnlineSessionSetting CustomSessionName;
	CustomSessionName.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
	CustomSessionName.Data = NickName.Append(TEXT("'s room"));
	
	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings.Get()->bIsDedicated = false;
	SessionSettings.Get()->bIsLANMatch = bUseLan;
	SessionSettings.Get()->bShouldAdvertise = true;
	SessionSettings.Get()->bUsesPresence = true;
	SessionSettings.Get()->NumPublicConnections = MaxPlayerCount;
	SessionSettings.Get()->Settings.Add(FName("SessionName"), CustomSessionName);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), FName(*NickName), *SessionSettings.Get());
}

void UMSBGameInstance::TryFindSession(bool bUseLan)
{
	GetSubsystemAndSessionInterface();
	if(OnlineSubsystem == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get online subsystem"));
		return;
	}
	if(SessionInterface == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get session interface"));
		return;
	}

	LastSearchResult = MakeShareable(new FOnlineSessionSearch());
	LastSearchResult->MaxSearchResults = 10;
	LastSearchResult->bIsLanQuery = bUseLan;
	LastSearchResult->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black,
				TEXT("세션 찾기를 시작합니다."));
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSearchResult.ToSharedRef());
}


void UMSBGameInstance::OnSessionCreate(FName SessionName, bool bWasSucceed)
{
	if(!bWasSucceed)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed to create session"));
		return;
	}
	GetWorld()->ServerTravel(DessertMap);
}

void UMSBGameInstance::OnFindSessionComplete(bool bSucceed)
{
	if(bSucceed)
	{
		OnSessionSearchCompleteWithResults.ExecuteIfBound(LastSearchResult->SearchResults, bSucceed);
	}
	else
	{
		auto EmptyArr = TArray<FOnlineSessionSearchResult>();
		OnSessionSearchCompleteWithResults.ExecuteIfBound(EmptyArr, bSucceed);
	}
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
}

void UMSBGameInstance::GetSubsystemAndSessionInterface()
{
	OnlineSubsystem = IOnlineSubsystem::Get();
	SessionInterface = OnlineSubsystem->GetSessionInterface().Get();
}

