// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "TCPSocketBase.h"
#include "TCPServer.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNewClientJoinLobby)
DECLARE_MULTICAST_DELEGATE(FOnClientExitLobby)
/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API UTCPServer : public UTCPSocketBase
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;
	
	static TSharedRef<FInternetAddr> GetLocalHostIPv4();

	void StartHost();
private:
	void FindAvailablePort(TSharedRef<FInternetAddr> Addr);

	FSocket* ListenSocket;
	TArray<FSocket*> ConnectionSockets;

public:
	FOnNewClientJoinLobby OnNewClientJoinLobby;
	FOnClientExitLobby OnClientExitLobby;
};
