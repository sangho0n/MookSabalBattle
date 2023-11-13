// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Blueprint/UserWidget.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "ServerBrowserItemUI.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionSelected, TSharedPtr<FOnlineSessionSearchResult>)

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API UServerBrowserItemUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
private:
	UCheckBox* CheckBox_Select;
	UTextBlock* TextBlock_PlayerCount;
	UTextBlock* TextBlock_SessionName;

public:
	FOnSessionSelected OnSessionSelected;

	TSharedPtr<FOnlineSessionSearchResult> Session;

	void SetCheck(TSharedPtr<FOnlineSessionSearchResult> dummy, bool flag);

private:
	void OnSelected(bool bIsChecked);
};
