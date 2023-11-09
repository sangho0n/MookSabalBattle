// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPServer.h"

#include "SocketSubsystem.h"
#include "Common/TcpSocketBuilder.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "MookSabalBattle/MookSabalBattleGameModeBase.h"

FSocket* UTCPServer::ListenSocket = nullptr;
TArray<FSocket*> UTCPServer::ConnectionSockets = {};

void UTCPServer::CloseSocket()
{
	isSocketConnectionValid = false;
	if(nullptr != ListenSocket)
	{
		ListenSocket->Shutdown(ESocketShutdownMode::ReadWrite);
		ListenSocket->Close();
		SocketSubsystem->DestroySocket(ListenSocket);
		ListenSocket = nullptr;
	}

	if(!ConnectionSockets.IsEmpty())
	{
		for (auto Sock : ConnectionSockets)
		{
			Sock->Shutdown(ESocketShutdownMode::ReadWrite);
			Sock->Close();
			SocketSubsystem->DestroySocket(Sock);
		}
		ConnectionSockets.Empty();
	}
}


TSharedRef<FInternetAddr> UTCPServer::GetLocalHostIPv4()
{
	bool bCanBindAll;
	TSharedRef<FInternetAddr> Addr = SocketSubsystem->GetLocalHostAddr(*GLog, bCanBindAll);
	return Addr;
}

void UTCPServer::StartHost()
{
	ConnectionSockets = {};
	// 서버 소켓 생성
	auto Addr = GetLocalHostIPv4(); FindAvailablePort(Addr);
	auto Endpoint = FIPv4Endpoint(Addr);
	ListenSocket = FTcpSocketBuilder(TEXT("YourSocketName"))
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.Listening(3);
	
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("호스팅 시작"));

	isSocketConnectionValid = true;
	AsyncTask(ENamedThreads::AnyThread, []()->void{ListenClient();});
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

void UTCPServer::ListenClient()
{
	// start listen
	while (isSocketConnectionValid)
	{
		TSharedRef<FInternetAddr> RemoteAddress = SocketSubsystem->CreateInternetAddr();
		auto ConnectionSocket = ListenSocket->Accept(*RemoteAddress, TEXT("Connection"));
		
		// 클라이언트와의 통신을 처리합니다.
		while (isSocketConnectionValid && ConnectionSocket != nullptr)
		{
			// 메시지 수신 및 처리
			uint32 DataSize = sizeof(FTCPMessage);
			uint8* ReceivedData = new uint8[DataSize];
		
			if (isSocketConnectionValid && ConnectionSocket->HasPendingData(DataSize))
			{
				int32 ReadData = 0;
				ConnectionSocket->Recv(ReceivedData, DataSize, ReadData);

			
				// 역직렬화
				auto DeserializedMessage = DeserializeToTCPMessage(ReceivedData);

				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("메시지 도착"));
		
				if(DeserializedMessage.Type == 0) // 0 means join request
					{
					ConnectionSockets.Add(ConnectionSocket);
					SendMessageTypeOf(ConnectionSocket, 2, ConnectionSockets.Num() + 1); // 2 means ack response

					for (auto Sock : ConnectionSockets)
					{
						SendMessageTypeOf(Sock, 3, ConnectionSockets.Num() + 1); // 3 means adjust player count
					}
					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black,
						TEXT("클라이언트의 조인 요청"));

				
					AsyncTask(ENamedThreads::GameThread, [DeserializedMessage]()->void
					{
						OnPlayerCountUpdate.Broadcast(ConnectionSockets.Num() + 1);
					});

					// TODO max player 박아놓은 거 고치기
					if((ConnectionSockets.Num() + 1) == 2)
					{
						AsyncTask(ENamedThreads::GameThread, [DeserializedMessage]()->void
						{
							OnMaxPlayerJoined.Broadcast(FString("dummy server ip"));
						});
					}
					}
				else
				{
					//MSB_LOG(Warning, TEXT("Undefined behavior of join"));
					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black,
						TEXT("Undefined behavior of join"));

				}
			}
		}
	}

	CloseSocket();
}

