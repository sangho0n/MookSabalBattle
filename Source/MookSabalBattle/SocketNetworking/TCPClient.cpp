// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPClient.h"

#include "Common/TcpSocketBuilder.h"
#include "Interfaces/IPv4/IPv4Address.h"

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

	FTCPMessage Join(0);
	int32 BytesShouldBeSent = sizeof(FTCPMessage);
	int32 BytesSent = 0;

	FBufferArchive Arch;
	Arch << Join;
	uint8* BufferStart = Arch.GetData();

	bool flag = false;
	while (BytesShouldBeSent > 0)
	{
		if(Socket->Send(BufferStart, BytesShouldBeSent, BytesSent))
		{
			BytesShouldBeSent -= BytesSent;
			BufferStart += BytesSent;
			flag = true;
		}
		else
		{
			flag = false;
		}
	}

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
			check(DeserializedMessage.Type==2); // TODO Why received ack type is 0?
		}
	}
	
	delete[] ReceivedData;	
	
}
