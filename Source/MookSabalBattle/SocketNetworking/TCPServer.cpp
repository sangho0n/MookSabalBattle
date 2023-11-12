// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPServer.h"

#include "SocketSubsystem.h"
#include "Common/TcpSocketBuilder.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "MookSabalBattle/MookSabalBattleGameModeBase.h"

TSharedRef<FInternetAddr> UTCPServer::GetLocalHostIPv4()
{
	bool bCanBindAll;
	TSharedRef<FInternetAddr> Addr = SocketSubsystem->GetLocalHostAddr(*GLog, bCanBindAll);
	return Addr;
}

void UTCPServer::PostInitProperties()
{
	Super::PostInitProperties();
	
	ConnectionSockets = {};
}


void UTCPServer::StartHost()
{
	// 서버 소켓 생성
	auto Addr = GetLocalHostIPv4(); FindAvailablePort(Addr);
	auto Endpoint = FIPv4Endpoint(Addr);
	FSocket* ListenerSocket = FTcpSocketBuilder(TEXT("YourSocketName"))
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.Listening(3);
	
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("호스팅 시작"));

	// start listen
	while (true)
	{
		TSharedRef<FInternetAddr> RemoteAddress = SocketSubsystem->CreateInternetAddr();
		FSocket* ConnectionSocket = ListenerSocket->Accept(*RemoteAddress, TEXT("Connection"));
		
		// 클라이언트와의 통신을 처리합니다.
		if (ConnectionSocket != nullptr)
		{
			// 메시지 수신 및 처리
			uint32 DataSize = sizeof(FTCPMessage);
			uint8* ReceivedData = new uint8[DataSize];
			
			if (ConnectionSocket->HasPendingData(DataSize))
			{
				int32 ReadData = 0;
				ConnectionSocket->Recv(ReceivedData, DataSize, ReadData);
			}
		
			// 역직렬화
			auto DeserializedMessage = DeserializeToTCPMessage(ReceivedData);

			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("메시지 도착"));
			
			if(DeserializedMessage.Type == 0) // 0 means join request
			{
				ConnectionSockets.Add(ConnectionSocket);
				FTCPMessage Ack(2, ConnectionSockets.Num()); // 2 means ack response
				int32 ByteShouldBeSent = sizeof(FTCPMessage);
				FBufferArchive AckArch;
				AckArch << Ack;
				
				uint8* BufferStart = AckArch.GetData();
				
				while(ByteShouldBeSent > 0)
				{
					int32 ByteSent;
					ConnectionSocket->Send(BufferStart, ByteShouldBeSent, ByteSent);
					
					ByteShouldBeSent -= ByteSent;
					BufferStart += ByteSent;
				}

				delete[] BufferStart;
				OnNewClientJoinLobby.Broadcast();
			}
			else
			{
				//MSB_LOG(Warning, TEXT("Undefined behavior of join"));
			}
		}
	}
}

void UTCPServer::FindAvailablePort(TSharedRef<FInternetAddr> Addr)
{
	for(auto Port : PortCandidates)
	{
		if(Addr->IsPortValid(Port))
		{
			Addr->SetPort(Port); return;
		}
	}
	// TODO : 모든 포트가 불가능한 경우 에러처리
}

