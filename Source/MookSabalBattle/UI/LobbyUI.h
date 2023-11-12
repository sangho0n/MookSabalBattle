// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "MookSabalBattle/SocketNetworking/TCPClient.h"
#include "MookSabalBattle/SocketNetworking/TCPServer.h"
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

	UCanvasPanel* Canvas_Waiting;
	UWidget* Image_Waiting;
	UTextBlock* Text_PlayerCount;
	UButton* Button_Exit;

	UCanvasPanel* Canvas_OnJoin;
	UEditableTextBox* Text_Address;
	UEditableTextBox* Text_NickNameCL;
	UButton* Button_ConfirmJoin;
	
	UCanvasPanel* Canvas_OnHost;
	UTextBlock* Text_HostIP;
	UEditableTextBox* Text_NickNameSrv;
	UButton* Button_ConfirmHost;

	UFUNCTION()
	void HostPressed();

	void ShowOnHostCanvas();
	
	void ShowWaitingCanvas();

	void TryHost(FString IP, FString NickName);

	UFUNCTION()
	void JoinPressed();

	void ShowOnJoinCanvas();

	void TryJoin(FString ServerIP, FString NickName);

	UFUNCTION()
	void ExitPressed();
	
	void Exit();

	UFUNCTION()
	void ConfirmJoinPressed();
	UFUNCTION()
	void ConfirmHostPressed();

	void UpdateWaitCanvas(float InDeltaTime);

	UFUNCTION()
	void UpdatePlayerCount(int32 CurrentCount);

	bool bStartWaiting;
	bool bIsHost;
	float AccWaitTime;

	UTCPServer* ServerManager;
	UTCPClient* ClientManager;

	UFUNCTION()
	void EnterGameOnServer();
	UFUNCTION()
	void EnterGameOnClient();
};
