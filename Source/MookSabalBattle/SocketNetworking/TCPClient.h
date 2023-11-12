// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "TCPSocketBase.h"
#include "TCPClient.generated.h"

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API UTCPClient : public UTCPSocketBase
{
	GENERATED_BODY()

public:
	static void CloseSocket();
	
	static void Join(FString ServerIP);
	
	static void Exit();
};
