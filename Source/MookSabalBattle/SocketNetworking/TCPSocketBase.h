// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Engine/NetConnection.h"
#include "TCPSocketBase.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerCountUpdate, int32)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxPlayerJoined, FString, ServerIP);
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
	// 3 : adjust player count
	// 4 : expel
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
class MOOKSABALBATTLE_API UTCPSocketBase : public UObject
{
	GENERATED_BODY()
public:
	UTCPSocketBase(const FObjectInitializer& ObjectInitializer);
	
	static void CloseSocket();
	
	static FString ReceiveMessage();
	
protected:
	static FSocket* Socket;

	static ISocketSubsystem* SocketSubsystem;

	static TArray<int32> PortCandidates;
	
	static FTCPMessage DeserializeToTCPMessage(uint8* MemLoc);

	static TArray<uint8> ConvertMemLocToArray(uint8* MemLoc);

	static void SendMessageTypeOf(FSocket* Sock, uint32 Type, uint32 PlayerCount=0);

	static bool isSocketConnectionValid;

public:
	static FOnPlayerCountUpdate OnPlayerCountUpdate;
	static FOnMaxPlayerJoined OnMaxPlayerJoined;
};
