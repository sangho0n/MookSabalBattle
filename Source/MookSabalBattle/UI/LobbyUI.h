// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/Slider.h"
#include "LobbyUI.generated.h"

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API ULobbyUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UButton* Button_Host;
	UButton* Button_Join;
	UButton* Button_Back_1;
	UButton* Button_Back_2;

	UCanvasPanel* Canvas_OnJoin;
	UButton* Button_Refresh;
	USlider* Slider_MaxPlayer;
	UCheckBox* CheckBox_UseLan;
	UScrollBox* ScrollBox_ServerBrowserList;
	UEditableTextBox* Text_NickNameCL;
	UButton* Button_ConfirmJoin;
	
	UCanvasPanel* Canvas_OnHost;
	UEditableTextBox* Text_NickNameSrv;
	UTextBlock* Text_MaxPlayer;
	UButton* Button_ConfirmHost;

	UFUNCTION()
	void HostPressed();

	void ShowOnHostCanvas();

	void TryHost(FString NickName, int32 MaxPlayerCount, bool bUseLan);

	UFUNCTION()
	void JoinPressed();

	void ShowOnJoinCanvas();

	void TryJoin(FString NickName, TSharedPtr<FOnlineSessionSearchResult> Session);

	UFUNCTION()
	void BackPressed();

	UFUNCTION()
	void ConfirmJoinPressed();
	UFUNCTION()
	void ConfirmHostPressed();

	UFUNCTION()
	void OnMaxPlayerChanged(float ratio);

public:
	TSharedPtr<FOnlineSessionSearchResult> SelectedSession;

private:
	void ResetSessionCheckState(TSharedPtr<FOnlineSessionSearchResult> NewSelectedSession);
};
