// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerBrowserItemUI.h"

void UServerBrowserItemUI::NativeConstruct()
{
	Super::NativeConstruct();

	this->CheckBox_Select = Cast<UCheckBox>(GetWidgetFromName(TEXT("CheckBox_Select")));
	this->TextBlock_PlayerCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_PlayerCount")));
	this->TextBlock_SessionName = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_SessionName")));

	CheckBox_Select->OnCheckStateChanged.AddUniqueDynamic(this, &UServerBrowserItemUI::OnSelected);
}

void UServerBrowserItemUI::NativeDestruct()
{
	OnSessionSelected.Clear();
	Session.Reset();
	Super::NativeDestruct();
}

void UServerBrowserItemUI::SetInitialData(FString SessionName, const FOnlineSessionSearchResult &OnlineSession)
{
	int32 currentPlayer = OnlineSession.Session.SessionSettings.NumPublicConnections - OnlineSession.Session.NumOpenPublicConnections;
	
	TextBlock_SessionName->SetText(FText::FromString(SessionName));
	// null subsystem은 플레이어가 세션에 참가해도 NumOpenPublicConnections가 감소하지 않음!
	TextBlock_PlayerCount->SetText(FText::FromString(FString::Printf(TEXT("null subsys"), OnlineSession.Session.SessionSettings.NumPublicConnections, OnlineSession.Session.NumOpenPublicConnections)));
	//TextBlock_PlayerCount->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), currentPlayer, OnlineSession.Session.SessionSettings.NumPublicConnections)));
	Session = MakeShared<FOnlineSessionSearchResult>(OnlineSession);
}


void UServerBrowserItemUI::SetCheck(bool flag)
{
	CheckBox_Select->SetIsChecked(flag);
}

void UServerBrowserItemUI::OnSelected(bool bIsChecked)
{
	if(!bIsChecked) return;

	CheckBox_Select->SetIsChecked(bIsChecked);
	OnSessionSelected.Broadcast(Session);
}
