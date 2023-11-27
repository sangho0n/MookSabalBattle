// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Blueprint/UserWidget.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "OnlineSessionSettings.h"
#include "ServerBrowserItemUI.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionSelected, TSharedPtr<FOnlineSession>)

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
	void SetInitialData(FString SessionName, FOnlineSession OnlineSession);
	
	FOnSessionSelected OnSessionSelected;

	TSharedPtr<FOnlineSession> Session;

	void SetCheck(bool flag);

private:
	UFUNCTION()
	void OnSelected(bool bIsChecked);
};
