// Fill out your copyright notice in the Description page of Project Settings.


#include "MSBGameInstance.h"

#include "Blueprint/UserWidget.h"
#include "Online/OnlineSessionNames.h"

UMSBGameInstance::UMSBGameInstance()
{
	ConstructorHelpers::FClassFinder<UUserWidget> LOADING(TEXT("/Game/Main/UI/Loading.Loading_C"));
	if(LOADING.Succeeded())
	{
		LoadingWidgetClass = LOADING.Class;
	}
}


void UMSBGameInstance::Init()
{
	Super::Init();
	GetSubsystemAndSessionInterface();

	SessionInterface->OnCreateSessionCompleteDelegates.RemoveAll(this);
	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMSBGameInstance::OnSessionCreate);
	SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMSBGameInstance::OnFindSessionComplete);
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMSBGameInstance::OnJoinSessionComplete);
	
	DessertMap = FString("/Game/Main/Maps/Map_Dessert");

	OnLoading.BindLambda([this]()-> void
	{
		if(LoadingWidgetClass->IsValidLowLevel())
			LoadingWidget = CreateWidget(this, LoadingWidgetClass); LoadingWidget->AddToViewport();
	});

	StopLoading.BindLambda([this]()->void
	{
		LoadingWidget->RemoveFromViewport();
		LoadingWidget = nullptr;
	});
	
	gRPCSubsystem = GetSubsystem<URegisterNicknameGrpcSubsystem>();
}

void UMSBGameInstance::HostGame(FString NickName, int32 MaxPlayerCount, bool bUseLan)
{
	OnLoading.Execute();
	GetSubsystemAndSessionInterface();
	if(OnlineSubsystem == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get online subsystem"));
		StopLoading.Execute();
		return;
	}
	if(SessionInterface == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get session interface"));
		StopLoading.Execute();
		return;
	}
	
	PlayerNickNames.Reset();
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
	SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), FName(*NickName), *SessionSettings.Get());
}

void UMSBGameInstance::TryFindSession(bool bUseLan)
{
	OnLoading.Execute();
	GetSubsystemAndSessionInterface();
	if(OnlineSubsystem == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get online subsystem"));
		StopLoading.Execute();
		return;
	}
	if(SessionInterface == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get session interface"));
		StopLoading.Execute();
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
	PlayerNickNames.Add(LocalPlayerNickName);
	GetWorld()->ServerTravel(DessertMap + TEXT("?listen?port=7777"));

	// get ipv4
	FString IPv4;
	SessionInterface->GetResolvedConnectString(SessionName, IPv4);
	gRPCSubsystem->StartListen(LocalPlayerNickName);
}

void UMSBGameInstance::OnFindSessionComplete(bool bSucceed)
{
	StopLoading.Execute();
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

void UMSBGameInstance::JoinSession(FString NickName, TWeakPtr<FOnlineSessionSearchResult> SelectedSession)
{
	OnLoading.Execute();
	GetSubsystemAndSessionInterface();
	if(OnlineSubsystem == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get online subsystem"));
		StopLoading.Execute();
		return;
	}
	if(SessionInterface == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get session interface"));
		StopLoading.Execute();
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

void UMSBGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type SessionType)
{
	GetSubsystemAndSessionInterface();
	if(OnlineSubsystem == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get online subsystem"));
		StopLoading.Execute();
		return;
	}
	if(SessionInterface == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Failed get session interface"));
		StopLoading.Execute();
		return;
	}
	if(SessionType != EOnJoinSessionCompleteResult::Success)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString("Cannot join session"));
		StopLoading.Execute();
		return;
	}

	APlayerController* const PlayerController = GetFirstLocalPlayerController();

	if(PlayerController && SessionInterface->GetResolvedConnectString(SessionName, TravelURL))
	{
		gRPCSubsystem->OnClientReceiveMessage.AddDynamic(this, &UMSBGameInstance::CheckNicknameDuplicity);
		LastJoinedSessionName = SessionName;
		UE_LOG(MookSablBattle, Log, TEXT("travel url : %s"), *TravelURL);
		gRPCSubsystem->RequestRegister(LocalPlayerNickName, TravelURL);
	}
}

void UMSBGameInstance::CheckNicknameDuplicity(bool bCanJoin)
{
	gRPCSubsystem->OnClientReceiveMessage.Clear();
	if(!bCanJoin)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString::Printf(TEXT("cannot join server")));
		// to assure that Slate accessing code runs in GameThread or SlateLoadingThread
		AsyncTask(ENamedThreads::GameThread, [this]() -> void
		{
			StopLoading.Execute();
			OnInvalidNickname.Execute();
		});
		// leave session as a client
		SessionInterface->DestroySession(LastJoinedSessionName);
		return;
	}
	APlayerController* const PlayerController = GetFirstLocalPlayerController();
	
	GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString::Printf(TEXT("trying client travel %s"), *TravelURL));
	PlayerController->ClientTravel(TravelURL, TRAVEL_Absolute);
}

void UMSBGameInstance::GetSubsystemAndSessionInterface()
{
	OnlineSubsystem = IOnlineSubsystem::Get();
	SessionInterface = OnlineSubsystem->GetSessionInterface().Get();
}
