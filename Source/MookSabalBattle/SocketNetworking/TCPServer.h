// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "TCPSocketBase.h"
#include "TCPServer.generated.h"

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API UTCPServer : public UTCPSocketBase
{
	GENERATED_BODY()

public:
	static void CloseSocket();
	
	static TSharedRef<FInternetAddr> GetLocalHostIPv4();

	static void StartHost();
private:
	static void FindAvailablePort(TSharedRef<FInternetAddr> Addr);

	static void ListenClient();

	static FSocket* ListenSocket;
	static TArray<FSocket*> ConnectionSockets;

};
