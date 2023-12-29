#include "RegisterNicknameServiceImpl.h"

#include "Async/Async.h"
#include <grpc/grpc.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

#include "MookSabalBattle/MSBGameInstance.h"

URegisterNicknameServiceImpl::URegisterNicknameServiceImpl()
{
	
}

URegisterNicknameServiceImpl::~URegisterNicknameServiceImpl()
{
	Channel.reset();
	ServiceStub.reset();
}

URegisterNicknameServiceImpl* URegisterNicknameServiceImpl::MakeNewObject(const FString& ServerUrl, const FString& Certificate, const FString& SslHostName)
{
	URegisterNicknameServiceImpl* Ret = NewObject<URegisterNicknameServiceImpl>();
	Ret->Init(ServerUrl, Certificate, SslHostName);
	return Ret;
}

void URegisterNicknameServiceImpl::Init(FString ServerUrl, const FString& Certificate, const FString& SslHostName)
{
	TArray<FString> Parsed;
	ServerUrl.ParseIntoArray(Parsed,TEXT(":"));
	ServerUrl = Parsed[0] + TEXT(":50051");
	
	grpc::string StringUrl = std::string(TCHAR_TO_UTF8(*ServerUrl));
	Channel = grpc::CreateChannel(StringUrl, grpc::InsecureChannelCredentials());
	ServiceStub = NicknameService::NewStub(Channel);
}


// called on server
grpc::Status URegisterNicknameServiceImpl::RegisterNickname(grpc::ServerContext* context, const NickName* request, RegisterResponse* response)
{
	// get nickname map
	FString Nickname = FString(request->value().c_str());
	TArray<FString> &PlayerNickNames = Cast<UMSBGameInstance>(GetWorld()->GetGameInstance())->PlayerNickNames;
	// find duplicate nickname
	
	UE_LOG(MookSablBattle, Log, TEXT("중복 여부 체크 %s"), PlayerNickNames.Contains(Nickname)?TEXT("true"):TEXT("false"));
	if(PlayerNickNames.Contains(Nickname))
	{
		response->set_bsucceed(false);
	}
	else
	{
		response->set_bsucceed(true);
		PlayerNickNames.Add(Nickname);
	}

	return grpc::Status::OK;
}
