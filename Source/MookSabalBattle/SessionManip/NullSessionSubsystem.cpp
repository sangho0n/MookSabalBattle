// Fill out your copyright notice in the Description page of Project Settings.


#include "NullSessionSubsystem.h"
#include "MookSabalBattle/MSBGameInstance.h"
#include "Online/OnlineSessionNames.h"

UNullSessionSubsystem::UNullSessionSubsystem()
{
	
}

void UNullSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency(URegisterNicknameGrpcSubsystem::StaticClass());
	GetSubsystemAndSessionInterface();

	SessionInterface->OnCreateSessionCompleteDelegates.RemoveAll(this);
	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNullSessionSubsystem::OnSessionCreate);
	SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNullSessionSubsystem::OnFindSessionComplete);
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNullSessionSubsystem::OnJoinSessionComplete);
	
	DessertMap = FString("/Game/Main/Maps/Map_Dessert");

	gRPCSubsystem = GetGameInstance()->GetSubsystem<URegisterNicknameGrpcSubsystem>();
}

void UNullSessionSubsystem::HostGame(FString& NickName, int32 MaxPlayerCount, bool bUseLan)
{
	Cast<UMSBGameInstance>(GetGameInstance())->StartLoading();
	GetSubsystemAndSessionInterface();
	if(OnlineSubsystem == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get online subsystem"));
		Cast<UMSBGameInstance>(GetGameInstance())->EndLoading();
		return;
	}
	if(SessionInterface == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get session interface"));
		Cast<UMSBGameInstance>(GetGameInstance())->EndLoading();
		return;
	}
	
	LocalPlayerNickName = NickName;

	FOnlineSessionSetting CustomSessionName;
	CustomSessionName.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
	CustomSessionName.Data = NickName.Append(TEXT("'s room"));
	
	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings.Get()->bIsDedicated = false;
	SessionSettings.Get()->bIsLANMatch = bUseLan;
	SessionSettings.Get()->bShouldAdvertise = true;
	SessionSettings.Get()->bUsesPresence = true;
	SessionSettings.Get()->NumPublicConnections = MaxPlayerCount; MaxPlayer = MaxPlayerCount;
	SessionSettings.Get()->Settings.Add(FName("SessionName"), CustomSessionName);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), *CustomSessionName.Data.ToString(), *SessionSettings.Get());
	// warning : null subsystem에서는 위 코드로는 직접 SessionName을 바꿀 수 없음!
}

void UNullSessionSubsystem::TryFindSession(bool bUseLan)
{
	
	Cast<UMSBGameInstance>(GetGameInstance())->StartLoading();
	GetSubsystemAndSessionInterface();
	if(OnlineSubsystem == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get online subsystem"));
		Cast<UMSBGameInstance>(GetGameInstance())->EndLoading();
		return;
	}
	if(SessionInterface == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get session interface"));
		Cast<UMSBGameInstance>(GetGameInstance())->EndLoading();
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


void UNullSessionSubsystem::OnSessionCreate(FName SessionName, bool bWasSucceed)
{
	if(!bWasSucceed)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed to create session"));
		return;
	}
	GetWorld()->ServerTravel(DessertMap + TEXT("?listen?port=7777?Nickname=") + LocalPlayerNickName);

	// get ipv4
	FString IPv4;
	SessionInterface->GetResolvedConnectString(SessionName, IPv4);
	gRPCSubsystem->StartListen(LocalPlayerNickName);
}

void UNullSessionSubsystem::OnFindSessionComplete(bool bSucceed)
{
	Cast<UMSBGameInstance>(GetGameInstance())->EndLoading();
	if(bSucceed && LastSearchResult.IsValid())
	{
		OnSessionSearchCompleteWithResults.ExecuteIfBound(LastSearchResult->SearchResults, bSucceed);
	}
	else
	{
		auto EmptyArr = TArray<FOnlineSessionSearchResult>();
		OnSessionSearchCompleteWithResults.ExecuteIfBound(EmptyArr, bSucceed);
	}
}

void UNullSessionSubsystem::JoinSession(const FString& NickName, TWeakPtr<FOnlineSessionSearchResult> SelectedSession)
{
	Cast<UMSBGameInstance>(GetGameInstance())->StartLoading();
	GetSubsystemAndSessionInterface();
	if(OnlineSubsystem == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get online subsystem"));
		Cast<UMSBGameInstance>(GetGameInstance())->EndLoading();
		return;
	}
	if(SessionInterface == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get session interface"));
		Cast<UMSBGameInstance>(GetGameInstance())->EndLoading();
		return;
	}
	
	auto LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	const FOnlineSessionSearchResult& SessionSearchResult = std::ref(*SelectedSession.Pin().Get());
	MaxPlayer = SessionSearchResult.Session.SessionSettings.NumPublicConnections;

	if(SessionSearchResult.IsValid())
	{
		int32 OpenConnectionCount = SessionSearchResult.Session.NumOpenPublicConnections;
		if(OpenConnectionCount > 0)
		{
			FName SessionName = FName(SessionSearchResult.Session.SessionSettings.Settings.FindRef("SessionName").Data.ToString());
		
			GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString::Printf(TEXT("Try joining to %s"), *SessionName.ToString()));

			LocalPlayerNickName = NickName;
			SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), SessionName, SessionSearchResult);
			return;
		}
	}

	GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Selected Session is not valid now"));
}

void UNullSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type SessionType)
{
	GetSubsystemAndSessionInterface();
	if(OnlineSubsystem == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get online subsystem"));
		Cast<UMSBGameInstance>(GetGameInstance())->EndLoading();
		return;
	}
	if(SessionInterface == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get session interface"));
		Cast<UMSBGameInstance>(GetGameInstance())->EndLoading();
		return;
	}
	if(SessionType != EOnJoinSessionCompleteResult::Success)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Cannot join session"));
		Cast<UMSBGameInstance>(GetGameInstance())->EndLoading();
		return;
	}

	APlayerController* const PlayerController = GetGameInstance()->GetFirstLocalPlayerController();

	if(PlayerController && SessionInterface->GetResolvedConnectString(SessionName, TravelURL))
	{
		gRPCSubsystem->OnClientReceiveMessage.AddDynamic(this, &UNullSessionSubsystem::CheckNicknameDuplicity);
		LastJoinedSessionName = SessionName;
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString::Printf(TEXT("travel url : %s"), *TravelURL));
		gRPCSubsystem->RequestRegister(LocalPlayerNickName, TravelURL);
	}
}

void UNullSessionSubsystem::CheckNicknameDuplicity(bool bCanJoin)
{
	gRPCSubsystem->OnClientReceiveMessage.Clear();
	if(!bCanJoin)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString::Printf(TEXT("cannot join server")));
		// to assure that Slate accessing code runs in GameThread or SlateLoadingThread
		AsyncTask(ENamedThreads::GameThread, [this]() -> void
		{
			Cast<UMSBGameInstance>(GetGameInstance())->EndLoading();
			OnInvalidNickname.Execute();
		});
		// leave session as a client
		SessionInterface->DestroySession(LastJoinedSessionName);
		return;
	}
	APlayerController* const PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
	
	GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString::Printf(TEXT("trying client travel %s"), *TravelURL));
	PlayerController->ClientTravel(TravelURL + TEXT("?Nickname=") + LocalPlayerNickName, TRAVEL_Absolute);
}

void UNullSessionSubsystem::GetSubsystemAndSessionInterface()
{
	OnlineSubsystem = IOnlineSubsystem::Get();
	SessionInterface = OnlineSubsystem->GetSessionInterface().Get();
}
