// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Engine/NetConnection.h"
#include "TCPSocketBase.generated.h"

/**
 * 
 */
USTRUCT()
struct FTCPMessage
{
	GENERATED_BODY()
	uint32 Type;
	// 0 : join
	// 1 : exit
	// 2 : ack
	// 3 : expel
	uint32 PlayerCount;
	
	FTCPMessage() : Type(0), PlayerCount(1){}
	FTCPMessage(uint32 Type, uint32 PlayerCount): Type(Type), PlayerCount(PlayerCount){}
	FTCPMessage(uint32 Type): Type(Type), PlayerCount(0){}

	friend FArchive& operator<< (FArchive& Ar, FTCPMessage& Message)
	{
		Ar << Message.Type;
		Ar << Message.PlayerCount;
		return Ar;
	}
};

UCLASS()
class MOOKSABALBATTLE_API UTCPSocketBase : public UNetConnection
{
	GENERATED_BODY()
public:
	UTCPSocketBase(const FObjectInitializer& ObjectInitializer);
	~UTCPSocketBase();
	
	virtual FString ReceiveMessage();
	
protected:
	FSocket* Socket;

	static ISocketSubsystem* SocketSubsystem;

	TArray<int32> PortCandidates = {11111};
	
	FTCPMessage DeserializeToTCPMessage(uint8* MemLoc);

	TArray<uint8> ConvertMemLocToArray(uint8* MemLoc);

};
