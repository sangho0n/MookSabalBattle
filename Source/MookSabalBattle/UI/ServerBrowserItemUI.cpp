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


void UServerBrowserItemUI::SetCheck(TSharedPtr<FOnlineSessionSearchResult> dummy, bool flag)
{
	CheckBox_Select->SetIsChecked(flag);
}

void UServerBrowserItemUI::OnSelected(bool bIsChecked)
{
	if(!bIsChecked) return;
	
	OnSessionSelected.Broadcast(Session);
}
