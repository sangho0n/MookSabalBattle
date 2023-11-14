// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyUI.h"
#include "ServerBrowserItemUI.h"
#include "MookSabalBattle/MSBGameInstance.h"

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
	this->ScrollBox_ServerBrowserList = Cast<UScrollBox>(GetWidgetFromName(TEXT("ScrollBox")));
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

	Button_ConfirmJoin->OnClicked.AddUniqueDynamic(this, &ULobbyUI::ConfirmJoinPressed);
	Button_ConfirmHost->OnClicked.AddUniqueDynamic(this, &ULobbyUI::ConfirmHostPressed);

	Slider_MaxPlayer->OnValueChanged.AddUniqueDynamic(this, &ULobbyUI::OnMaxPlayerChanged);
}

void ULobbyUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SelectedSession.Reset();
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
	auto GameInstance = Cast<UMSBGameInstance>(GetGameInstance());
	GameInstance->HostGame(NickName, MaxPlayerCount, bUseLan);
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

	// TODO 브라우징 전 ServerBrowserList 클리어
	// TODO 게임인스턴스에서 브라우징 후 ServerBrowserList에 addChild
	// TODO addChild할 때마다 해당 위젯 인스턴스의 OnSessionSelected와 ResetSessionCheckState 바인딩
}

void ULobbyUI::ConfirmJoinPressed()
{
	FString NickName = Text_NickNameCL->GetText().ToString();
	
	bool bIgnore = false;

	// validate host request before hosting
	if(NickName.Len() > 10)
	{
		bIgnore = true;
		Text_NickNameCL->SetText(FText::FromString(TEXT("Nickname cannot over 10 characters")));
		Text_NickNameCL->SetForegroundColor(FColor::Red);
	}


	if(!bIgnore)
	{
		TryJoin(NickName, SelectedSession);
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
		Text_NickNameSrv->SetText(FText::FromString(TEXT("Nickname cannot over 10 characters")));
		Text_NickNameSrv->SetForegroundColor(FColor::Red);
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

void ULobbyUI::TryJoin(FString NickName, TSharedPtr<FOnlineSessionSearchResult> Session)
{
	// TODO gameInstance에 세션 참가 메서드 작성 후 요청
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

void ULobbyUI::OnMaxPlayerChanged(float ratio)
{
	int32 value = ratio * 4 + 2;
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
			SessionWidgetItem->SetCheck(SelectedSession, true);
		else
			SessionWidgetItem->SetCheck(SelectedSession, false);
	}
}

