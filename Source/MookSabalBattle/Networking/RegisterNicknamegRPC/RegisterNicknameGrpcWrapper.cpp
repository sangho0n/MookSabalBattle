#include "RegisterNicknameGrpcWrapper.h"

#include <grpcpp/server_builder.h>

#include "MookSabalBattle/MookSabalBattle.h"

void URegisterNicknameGrpcWrapper::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	Nickname = MakeShareable(new NickName);
	Response = MakeShareable(new RegisterResponse);
}

void URegisterNicknameGrpcWrapper::Deinitialize()
{
	Super::Deinitialize();
	
	Nickname.Reset();
	Response.Reset();
	if(gRPCServer.get())
		gRPCServer->Shutdown();
	gRPCServer.reset();
}

// called on server
void URegisterNicknameGrpcWrapper::StartListen()
{
	FString ServerAddr = TEXT("0.0.0.0:50051");
	int selected_port = 0;
	RegisterNicknameService = MakeWeakObjectPtr<URegisterNicknameServiceImpl>(URegisterNicknameServiceImpl::MakeNewObject(ServerAddr));

	grpc::ServerBuilder builder;
	builder.AddListeningPort(TCHAR_TO_ANSI(*ServerAddr), grpc::InsecureServerCredentials(), &selected_port);
	builder.RegisterService(RegisterNicknameService.Get());
	gRPCServer = builder.BuildAndStart();
	UE_LOG(MookSablBattle, Log, TEXT("gRPC Server listening on %s %d"), *ServerAddr, selected_port);

	if(gRPCServer)
	{

		AsyncTask(ENamedThreads::AnyThread, [this]()
		{
			gRPCServer->Wait();
		});
	}
	else
	{
		UE_LOG(MookSablBattle, Log, TEXT("Failed to start grpc server"));	
	}
}

// called on client
void URegisterNicknameGrpcWrapper::RequestRegister(const FString NickFStr,const FString ServerUrl, const FString Certificate, const FString SslHostName)
{
	RegisterNicknameService = MakeWeakObjectPtr<URegisterNicknameServiceImpl>(URegisterNicknameServiceImpl::MakeNewObject(ServerUrl, Certificate, SslHostName));
	grpc::ClientContext* Context = new grpc::ClientContext();
	Nickname->set_value(std::string(TCHAR_TO_UTF8(*NickFStr)));

	UE_LOG(MookSablBattle, Log, TEXT("grpc stub in client is null? %s"), nullptr == RegisterNicknameService->ServiceStub.get() ? TEXT("true") : TEXT("false"));

	// async call gRPC
	RegisterNicknameService->ServiceStub->async()->RegisterNickname(Context, Nickname.Get(), Response.Get(), [this](grpc::Status status) -> void
	{
		if(status.ok())
		{
			OnClientReceiveMessage.Broadcast(Response->bsucceed());
			UE_LOG(MookSablBattle, Log, TEXT("gRPC call successful"));
		}
		else
		{
			OnClientReceiveMessage.Broadcast(false);
			UE_LOG(MookSablBattle, Error, TEXT("networking error!"));
		}
	});
	
	// auto status = RegisterNicknameService->ServiceStub->RegisterNickname(Context, *Nickname.Get(), Response.Get());
	//
	// 	if(status.ok())
	// 	{
	// 		OnClientReceiveMessage.Broadcast(Response->bsucceed());
	// 		UE_LOG(MookSablBattle, Log, TEXT("gRPC call successful"));
	// 	}
	// 	else
	// 	{
	// 		OnClientReceiveMessage.Broadcast(false);
	// 		UE_LOG(MookSablBattle, Error, TEXT("networking error!"));
	// 	}
}
