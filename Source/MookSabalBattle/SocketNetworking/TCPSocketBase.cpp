// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPSocketBase.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

ISocketSubsystem* UTCPSocketBase::SocketSubsystem;

UTCPSocketBase::UTCPSocketBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);	
}

UTCPSocketBase::~UTCPSocketBase()
{
	if(nullptr != Socket)
		Socket->Close();
	SocketSubsystem->DestroySocket(Socket);
}

FString UTCPSocketBase::ReceiveMessage()
{
	FString Message(TEXT("default"));
	return Message;
}

FTCPMessage UTCPSocketBase::DeserializeToTCPMessage(uint8* MemLoc)
{
	
	FTCPMessage DeserializedMessage;
	auto ToBeDeserialized = ConvertMemLocToArray(MemLoc);
	FMemoryReader Reader(ToBeDeserialized, false);
	Reader << DeserializedMessage;
	return DeserializedMessage;
}

TArray<uint8> UTCPSocketBase::ConvertMemLocToArray(uint8* MemLoc)
{
	TArray<uint8> Ret = {};
	for(int i = 0; i < sizeof(FTCPMessage); i++)
	{
		Ret.Add(*(MemLoc + i));
	}
	return Ret;
}

void UTCPSocketBase::SendMessageTypeOf(FSocket* Sock, uint32 Type, uint32 PlayerCount)
{
	FTCPMessage Ack(Type, PlayerCount); // 2 means ack response
	int32 ByteShouldBeSent = sizeof(FTCPMessage);
	FBufferArchive AckArch;
	AckArch << Ack;
				
	uint8* BufferStart = AckArch.GetData();
				
	while(ByteShouldBeSent > 0)
	{
		int32 ByteSent;
		Sock->Send(BufferStart, ByteShouldBeSent, ByteSent);
					
		ByteShouldBeSent -= ByteSent;
		BufferStart += ByteSent;
	}

	delete[] BufferStart;
}
