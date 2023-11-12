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
	virtual void PostInitProperties() override;

	~UTCPServer();
	
	static TSharedRef<FInternetAddr> GetLocalHostIPv4();

	void StartHost();
private:
	void FindAvailablePort(TSharedRef<FInternetAddr> Addr);

	void ListenClient();

	FSocket* ListenSocket;
	TArray<FSocket*> ConnectionSockets;

};
