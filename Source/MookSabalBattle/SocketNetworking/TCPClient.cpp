// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPClient.h"

#include "Common/TcpSocketBuilder.h"
#include "Interfaces/IPv4/IPv4Address.h"

UTCPClient::~UTCPClient()
{
	UTCPSocketBase::~UTCPSocketBase();
}


void UTCPClient::Join(FString ServerIP)
{
	// 클라이언트 소켓 생성
	FIPv4Address ServerLocalAddr; ServerLocalAddr.Parse(ServerIP, ServerLocalAddr);
	int32 ServerPort = 11111;

	TSharedRef<FInternetAddr> ServerAddress = SocketSubsystem->CreateInternetAddr();
	ServerAddress->SetIp(ServerLocalAddr.Value);
	ServerAddress->SetPort(ServerPort);

	Socket = FTcpSocketBuilder(TEXT("YourClientSocketName"))
		.AsReusable().BoundToPort(11112);
	Socket->Connect(ServerAddress.Get());

	// 서버에 데이터 보내기
	SendMessageTypeOf(Socket, 0);

	// 서버로부터 응답 받기
	uint32 DataSize = sizeof(FTCPMessage);
	uint8* ReceivedData = new uint8[DataSize];

	while(true)
	{
		if(Socket->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromSeconds(5)))
		{
			int32 ReadData = 0;
			Socket->Recv(ReceivedData, DataSize, ReadData);

			// 역직렬화
			auto DeserializedMessage = DeserializeToTCPMessage(ReceivedData);
		
			delete[] ReceivedData;
			if(DeserializedMessage.Type==2) // ack
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black,
					TEXT("메시지 타입 : %d") + DeserializedMessage.Type);
			}
			else if (DeserializedMessage.Type==3) // adjust player count
			{
				AsyncTask(ENamedThreads::GameThread, [this, DeserializedMessage]()->void
				{
					OnPlayerCountUpdate.Broadcast(DeserializedMessage.PlayerCount);
				});
				// TODO max player 박아놓은 거 바꾸기
				if(DeserializedMessage.PlayerCount == 2)
				{
					AsyncTask(ENamedThreads::GameThread, [this, ServerIP]()->void
					{
						OnMaxPlayerJoined.Broadcast(ServerIP);
					});
				}
			}
		}
	}
	
}
