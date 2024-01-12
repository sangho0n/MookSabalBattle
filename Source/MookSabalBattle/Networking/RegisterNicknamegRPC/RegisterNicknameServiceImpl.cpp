#include "RegisterNicknameServiceImpl.h"

#include <grpc/grpc.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

#include "UMG.h"
#include "UMGStyle.h"
#include "Slate/SObjectWidget.h"
#include "IUMGModule.h"
#include "Blueprint/UserWidget.h"

RegisterNicknameServiceImpl::RegisterNicknameServiceImpl()
{
	
}

RegisterNicknameServiceImpl::~RegisterNicknameServiceImpl()
{
	Channel.reset();
	ServiceStub.reset();
}

void RegisterNicknameServiceImpl::Init(const FString PlayerNickname, FString ServerUrl, const FString& Certificate, const FString& SslHostName)
{
	TArray<FString> Parsed;
	ServerUrl.ParseIntoArray(Parsed,TEXT(":"));
	ServerUrl = Parsed[0] + TEXT(":50051");
	
	grpc::string StringUrl = std::string(TCHAR_TO_UTF8(*ServerUrl));
	Channel = grpc::CreateChannel(StringUrl, grpc::InsecureChannelCredentials());
	ServiceStub = NicknameService::NewStub(Channel);

	PlayerNicknames.Add(PlayerNickname);
}


// called on server
grpc::Status RegisterNicknameServiceImpl::RegisterNickname(grpc::ServerContext* context, const NickName* request, RegisterResponse* response)
{
	// get nickname map
	FString Nickname = FString(request->value().c_str());
	
	// find duplicate nickname
	if(PlayerNicknames.Contains(Nickname))
	{
		response->set_bsucceed(false);
	}
	else
	{
		response->set_bsucceed(true);
		PlayerNicknames.Add(Nickname);
	}

	return grpc::Status::OK;
}
