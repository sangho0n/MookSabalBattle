#pragma once

#include "MookSabalBattle/MookSabalBattle.h"
#include "Proto/RegisterNickname.grpc.pb.h"

/**
 * 
 */
class MOOKSABALBATTLE_API RegisterNicknameServiceImpl final : public NicknameService::Service
{
public:
	RegisterNicknameServiceImpl();
	~RegisterNicknameServiceImpl();

	void Init(const FString PlayerNickname, FString ServerUrl, const FString& Certificate = TEXT("null"), const FString& SslHostName = TEXT("localhost"));
	
// rpc
	grpc::Status RegisterNickname(grpc::ServerContext* context, const NickName* request, RegisterResponse* response) override;
	
// client
	std::unique_ptr<NicknameService::Stub> ServiceStub;

// temporal structure that contains player nicknames
	TArray<FString> PlayerNicknames;
	
private:
	std::shared_ptr<grpc::Channel> Channel;
};
