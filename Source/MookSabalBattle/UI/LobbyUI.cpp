// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyUI.h"

#include "MookSabalBattle/MSBGameInstance.h"

bool ULobbyUI::bIsHost;

void ULobbyUI::NativeConstruct()
{
	Super::NativeConstruct();

	this->Button_Host = Cast<UButton>(GetWidgetFromName(TEXT("BT_Host")));
	this->Button_Join = Cast<UButton>(GetWidgetFromName(TEXT("BT_Join")));

	this->Canvas_Waiting = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Canvas_Waiting")));
	this->Image_Waiting = GetWidgetFromName(TEXT("Image_Waiting"));
	this->Text_PlayerCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text_PlayerCount")));
	this->Button_Exit = Cast<UButton>(GetWidgetFromName(TEXT("BT_Exit")));

	this->Canvas_OnJoin = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Canvas_OnJoin")));
	this->Text_Address = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Text_Address")));
	this->Text_NickNameCL = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Text_NickNameCL")));
	this->Button_ConfirmJoin = Cast<UButton>(GetWidgetFromName(TEXT("BT_ConfirmJoin")));

	this->Canvas_OnHost = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Canvas_OnHost")));
	this->Text_HostIP = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text_HostIP")));
	this->Text_NickNameSrv = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Text_NickNameSrv")));
	this->Button_ConfirmHost = Cast<UButton>(GetWidgetFromName(TEXT("BT_ConfirmHost")));

	Canvas_Waiting->SetVisibility(ESlateVisibility::Hidden);
	Canvas_OnJoin->SetVisibility(ESlateVisibility::Hidden);
	Canvas_OnHost->SetVisibility(ESlateVisibility::Hidden);

	Button_Host->SetIsEnabled(true);
	Button_Join->SetIsEnabled(true);

	bStartWaiting = false;
	bIsHost = false;
	AccWaitTime = 0.0f;
	
	Button_Host->OnClicked.AddUniqueDynamic(this, &ULobbyUI::HostPressed);
	Button_Join->OnClicked.AddUniqueDynamic(this, &ULobbyUI::JoinPressed);

	Button_Exit->OnClicked.AddUniqueDynamic(this, &ULobbyUI::ExitPressed);

	Button_ConfirmJoin->OnClicked.AddUniqueDynamic(this, &ULobbyUI::ConfirmJoinPressed);
	Button_ConfirmHost->OnClicked.AddUniqueDynamic(this, &ULobbyUI::ConfirmHostPressed);
}

void ULobbyUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void ULobbyUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(bStartWaiting) UpdateWaitCanvas(InDeltaTime);
}

void ULobbyUI::HostPressed()
{
	Button_Join->SetIsEnabled(false);
	Button_Host->SetIsEnabled(false);
	ShowOnHostCanvas();
}

void ULobbyUI::ShowOnHostCanvas()
{
	Canvas_OnHost->SetVisibility(ESlateVisibility::Visible);
	auto HostIP = UTCPServer::GetLocalHostIPv4();
	Text_HostIP->SetText(FText::FromString(HostIP->ToString(false)));
}

void ULobbyUI::ShowWaitingCanvas()
{
	Canvas_Waiting->SetVisibility(ESlateVisibility::Visible);
	Button_Join->SetIsEnabled(false);
	Button_Host->SetIsEnabled(false);
	Button_Exit->SetIsEnabled(true);
	bStartWaiting = true;
}


void ULobbyUI::TryHost(FString IP, FString NickName)
{
	MSB_LOG(Warning, TEXT("try host"));
	bIsHost = true;
	// wait until all players join
	auto GameInstance = Cast<UMSBGameInstance>(GetGameInstance());
	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, GameInstance]()->void
       	{
       		UTCPServer::OnPlayerCountUpdate.AddUObject(this, &ULobbyUI::UpdatePlayerCount);
			check(GameInstance);
			UTCPServer::OnMaxPlayerJoined.AddDynamic(GameInstance, &UMSBGameInstance::EnterGameOnServer);
       		UTCPServer::StartHost();
       	});
}


void ULobbyUI::JoinPressed()
{
	ShowOnJoinCanvas();
}

void ULobbyUI::ShowOnJoinCanvas()
{
	Button_Host->SetIsEnabled(false);
	Button_Join->SetIsEnabled(false);
	Canvas_OnJoin->SetVisibility(ESlateVisibility::Visible);
	Button_ConfirmJoin->SetIsEnabled(true);
}

void ULobbyUI::ConfirmJoinPressed()
{
	FString IP = Text_Address->GetText().ToString();
	FString NickName = Text_NickNameCL->GetText().ToString();
	TArray<FString> Chunks = {};
	bool bIgnore = false;

	// 서버 접속 요청 전 검증
	if(NickName.Len() == 0 || NickName.Len() > 10)
	{
		Text_NickNameCL->SetText(FText::FromString(TEXT("Not Valid. Try again")));
		bIgnore = true;
	}

	IP.ParseIntoArray(Chunks, TEXT("."));
	if(Chunks.Num() != 4) bIgnore = true;
	
	for(auto elem : Chunks)
	{
		if(!elem.IsNumeric()) bIgnore = true;
	}

	if(!bIgnore)
	{
		TryJoin(IP, NickName);
		Canvas_OnJoin->SetVisibility(ESlateVisibility::Hidden);
		ShowWaitingCanvas();
	}
	else
	{
		MSB_LOG(Error, TEXT("조인 실패"));
		ShowOnJoinCanvas();
	}
}

void ULobbyUI::ConfirmHostPressed()
{
	FString IP = Text_HostIP->GetText().ToString();
	FString NickName = Text_NickNameSrv->GetText().ToString();
	TArray<FString> Chunks = {};
	bool bIgnore = false;

	// 서버 접속 요청 전 검증
	if(NickName.Len() == 0 || NickName.Len() > 10)
	{
		Text_NickNameSrv->SetText(FText::FromString(TEXT("Not Valid. Try again")));
		bIgnore = true;
	}

	if(!bIgnore)
	{
		TryHost(IP, NickName);
		Canvas_OnHost->SetVisibility(ESlateVisibility::Hidden);
		ShowWaitingCanvas();
	}
	else
	{
		MSB_LOG(Error, TEXT("호스팅 실패."));
		ShowOnHostCanvas();
	}
}

void ULobbyUI::TryJoin(FString ServerIP, FString NickName)
{
	MSB_LOG(Warning, TEXT("try join"));
	Text_HostIP->SetText(FText::FromString(ServerIP));
	// wait until all players join
	auto GameInstance = Cast<UMSBGameInstance>(GetGameInstance());
	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, ServerIP, GameInstance]()->void
		   {
		       UTCPClient::OnPlayerCountUpdate.AddUObject(this, &ULobbyUI::UpdatePlayerCount); 
		       UTCPClient::OnMaxPlayerJoined.AddDynamic(GameInstance, &UMSBGameInstance::EnterGameOnClient);;
			   UTCPClient::Join(ServerIP);
		   });
}

void ULobbyUI::ExitPressed()
{
	Exit();
	AccWaitTime = 0.0f;
	bStartWaiting = false;
	Button_Exit->SetIsEnabled(false);
	Canvas_Waiting->SetVisibility(ESlateVisibility::Hidden);
	Button_Host->SetIsEnabled(true);
	Button_Join->SetIsEnabled(true);
}


void ULobbyUI::Exit()
{
	UTCPSocketBase::OnPlayerCountUpdate.Clear();
	UTCPSocketBase::OnMaxPlayerJoined.Clear();
	if(bIsHost) // TODO : static이라 에디터에서 둘 다 띄우면 분기 안됨 주의
	{
		UTCPServer::Exit();
	}
	else
	{
		UTCPClient::Exit();
	}
}

void ULobbyUI::UpdateWaitCanvas(float InDeltaTime)
{
	AccWaitTime += InDeltaTime;

	Image_Waiting->SetRenderTransformAngle(AccWaitTime * 100.0f);
}

void ULobbyUI::UpdatePlayerCount(int32 CurrentCount)
{
	// TODO : max player 2 박아놓은 거 수정
	Text_PlayerCount->SetText(FText::Format(
		FText::FromString("{0} / 3"), CurrentCount
	));
}

