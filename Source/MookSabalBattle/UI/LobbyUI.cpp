// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyUI.h"

#include "MookSabalBattle/SessionManip/NullSessionSubsystem.h"

void ULobbyUI::NativeConstruct()
{
	Super::NativeConstruct();

	this->Button_Host = Cast<UButton>(GetWidgetFromName(TEXT("BT_Host")));
	this->Button_Join = Cast<UButton>(GetWidgetFromName(TEXT("BT_Join")));
	this->Button_Back_1 = Cast<UButton>(GetWidgetFromName(TEXT("BT_Back_1")));
	this->Button_Back_2 = Cast<UButton>(GetWidgetFromName(TEXT("BT_Back_2")));

	this->Canvas_OnJoin = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Canvas_OnJoin")));
	this->Button_Refresh = Cast<UButton>(GetWidgetFromName(TEXT("BT_Refresh")));
	this->Slider_MaxPlayer = Cast<USlider>(GetWidgetFromName(TEXT("Slider_MaxPlayer")));
	this->CheckBox_UseLan = Cast<UCheckBox>(GetWidgetFromName(TEXT("CheckBox_UseLan")));
	this->ScrollBox_ServerBrowserList = Cast<UScrollBox>(GetWidgetFromName(TEXT("ScrollBox_RoomList")));
	this->Text_NickNameCL = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Text_NickNameCL")));
	this->Button_ConfirmJoin = Cast<UButton>(GetWidgetFromName(TEXT("BT_ConfirmJoin")));

	this->Canvas_OnHost = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Canvas_OnHost")));
	this->Text_NickNameSrv = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Text_NickNameSrv")));
	this->Text_MaxPlayer = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text_MaxPlayer")));
	this->Button_ConfirmHost = Cast<UButton>(GetWidgetFromName(TEXT("BT_ConfirmHost")));

	Canvas_OnJoin->SetVisibility(ESlateVisibility::Hidden);
	Canvas_OnHost->SetVisibility(ESlateVisibility::Hidden);

	Button_Host->SetIsEnabled(true);
	Button_Join->SetIsEnabled(true);
	
	Button_Host->OnClicked.AddUniqueDynamic(this, &ULobbyUI::HostPressed);
	Button_Join->OnClicked.AddUniqueDynamic(this, &ULobbyUI::JoinPressed);
	Button_Back_1->OnClicked.AddUniqueDynamic(this, &ULobbyUI::BackPressed);
	Button_Back_2->OnClicked.AddUniqueDynamic(this, &ULobbyUI::BackPressed);
	Button_Refresh->OnClicked.AddUniqueDynamic(this, &ULobbyUI::RefreshServerBrowser);

	Button_ConfirmJoin->OnClicked.AddUniqueDynamic(this, &ULobbyUI::ConfirmJoinPressed);
	Button_ConfirmHost->OnClicked.AddUniqueDynamic(this, &ULobbyUI::ConfirmHostPressed);

	Slider_MaxPlayer->OnValueChanged.AddUniqueDynamic(this, &ULobbyUI::OnMaxPlayerChanged);
}

void ULobbyUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SelectedSession.Reset();

	auto SessionSubsystem = GetGameInstance()->GetSubsystem<UNullSessionSubsystem>();
	SessionSubsystem->OnInvalidNickname.BindUObject(this, &ULobbyUI::NotifyInvalidNickname);
}

void ULobbyUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void ULobbyUI::HostPressed()
{
	Button_Host->SetIsEnabled(false);
	Button_Join->SetIsEnabled(false);
	ShowOnHostCanvas();
}

void ULobbyUI::ShowOnHostCanvas()
{
	Canvas_OnHost->SetVisibility(ESlateVisibility::Visible);
	Button_Back_2->SetIsEnabled(true);
	Text_NickNameSrv->SetForegroundColor(FColor::Black);
}


void ULobbyUI::TryHost(FString NickName, int32 MaxPlayerCount, bool bUseLan)
{
	auto SessionSubsystem = GetGameInstance()->GetSubsystem<UNullSessionSubsystem>();
	SessionSubsystem->HostGame(NickName, MaxPlayerCount, bUseLan);
}

void ULobbyUI::JoinPressed()
{
	SelectedSession.Reset();
	Button_Host->SetIsEnabled(false);
	Button_Join->SetIsEnabled(false);
	ShowOnJoinCanvas();
}

void ULobbyUI::ShowOnJoinCanvas()
{
	Canvas_OnJoin->SetVisibility(ESlateVisibility::Visible);
	Button_Back_1->SetIsEnabled(true);
	Button_Refresh->SetIsEnabled(true);
	Button_ConfirmJoin->SetIsEnabled(true);
	
	RefreshServerBrowser();
}

void ULobbyUI::ConfirmJoinPressed()
{
	FString NickName = Text_NickNameCL->GetText().ToString();
	
	bool bIgnore = false;

	// validate join request before hosting
	if(NickName.Len() > 10)
	{
		bIgnore = true;
		Text_NickNameCL->SetText(FText::FromString(TEXT("Nickname cannot over 10 characters")));
		Text_NickNameCL->SetForegroundColor(FColor::Red);
	}
	if(!SelectedSession.IsValid())
	{
		bIgnore = true;
		// TODO 세션 선택 안됐을 시 위젯 띄우기
	}


	if(!bIgnore)
	{
		TryJoin(NickName);
	}
}

void ULobbyUI::ConfirmHostPressed()
{
	FString NickName = Text_NickNameSrv->GetText().ToString();
	bool bUseLan = CheckBox_UseLan->IsChecked();
	int32 MaxPlayerCount = FCString::Atoi(*Text_MaxPlayer->GetText().ToString());
	bool bIgnore = false;

	// validate host request before hosting
	if(NickName.Len() > 10)
	{
		bIgnore = true;
		Text_NickNameSrv->SetError(FText::FromString(TEXT("Nickname cannot over 10 characters")));
	}
	if(!bUseLan)
	{
		// TODO this if branching point must be removed after implementing Server
		bIgnore = true;
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black,
					TEXT("지금은 원격 접속 불가능함"));
	}

	if(!bIgnore)
	{
		TryHost(NickName, MaxPlayerCount, bUseLan);
	}
}

void ULobbyUI::TryJoin(FString NickName)
{
	auto SessionSubsystem = GetGameInstance()->GetSubsystem<UNullSessionSubsystem>();
	SessionSubsystem->JoinSession(NickName, SelectedSession.ToWeakPtr());
}

void ULobbyUI::BackPressed()
{
	Canvas_OnHost->SetVisibility(ESlateVisibility::Hidden);
	Canvas_OnJoin->SetVisibility(ESlateVisibility::Hidden);
	Button_Back_1->SetIsEnabled(false);
	Button_Back_2->SetIsEnabled(false);
	Button_Host->SetIsEnabled(true);
	Button_Join->SetIsEnabled(true);
}

void ULobbyUI::RefreshServerBrowser()
{
	bool bUseLan = CheckBox_UseLan->IsChecked();
	// clear
	ScrollBox_ServerBrowserList->ClearChildren();
	
	// try find session
	// TODO 아래 코드는 스팀 연동 이후 지워질 코드
	if(!bUseLan)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black,
					TEXT("지금은 원격 접속 불가능함"));
		bUseLan = true;
	}
	
	auto SessionSubsystem = GetGameInstance()->GetSubsystem<UNullSessionSubsystem>();
	SessionSubsystem->TryFindSession(bUseLan);
	SessionSubsystem->OnSessionSearchCompleteWithResults.BindUObject(this, &ULobbyUI::SetSessionSearchResults);
}

void ULobbyUI::SetSessionSearchResults(TArray<FOnlineSessionSearchResult>& SessionSearchResults, bool bSucceed)
{
	if(bSucceed)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue,
					FString::Printf(TEXT("세션 찾기 성공함 : %d개"), SessionSearchResults.Num()));

		for(const FOnlineSessionSearchResult& SessionSearchResult : SessionSearchResults)
		{
			if(IsValid(ServerBrowserItemRef) && SessionSearchResult.Session.NumOpenPublicConnections > 0)
			{
				UServerBrowserItemUI* NewWidget = CreateWidget<UServerBrowserItemUI>(GetWorld(), ServerBrowserItemRef);
				NewWidget->AddToViewport();
				ScrollBox_ServerBrowserList->AddChild(NewWidget);
				NewWidget->OnSessionSelected.AddUObject(this, &ULobbyUI::ResetSessionCheckState);
				NewWidget->SetInitialData(SessionSearchResult.Session.SessionSettings.Settings.FindRef("SessionName").Data.ToString(), SessionSearchResult);
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue,
					TEXT("세션 찾기 실패"));
	}
	auto SessionSubsystem = GetGameInstance()->GetSubsystem<UNullSessionSubsystem>();
	SessionSubsystem->OnSessionSearchCompleteWithResults.Unbind();
}


void ULobbyUI::OnMaxPlayerChanged(float ratio)
{
	int32 value = ratio * 4 + 2;
	if((value & 1) == 1) value--;
	Text_MaxPlayer->SetText(FText::FromString(
		FString::Printf(TEXT("%d"), value)));
}

void ULobbyUI::ResetSessionCheckState(TSharedPtr<FOnlineSessionSearchResult> NewSelectedSession)
{
	SelectedSession = NewSelectedSession;
	for(const auto elem : ScrollBox_ServerBrowserList->GetAllChildren())
	{
		auto SessionWidgetItem = Cast<UServerBrowserItemUI>(elem);
		if(SelectedSession == SessionWidgetItem->Session)
			SessionWidgetItem->SetCheck(true);
		else
			SessionWidgetItem->SetCheck(false);
	}

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black,
				FString::Printf(TEXT("현재 선택된 세션 이름 %s"), *SelectedSession.Get()->Session.SessionSettings.Settings.FindRef("SessionName").Data.ToString()));
}

void ULobbyUI::NotifyInvalidNickname()
{
	Text_NickNameCL->SetError(FText::FromString(TEXT("This Nickname is already in use")));
}

