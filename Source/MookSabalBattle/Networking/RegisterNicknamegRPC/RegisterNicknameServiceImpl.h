#pragma once

#include "CoreMinimal.h"
#include "Proto/RegisterNickname.grpc.pb.h"
#include "RegisterNicknameServiceImpl.generated.h"

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API URegisterNicknameServiceImpl final : public UObject, public NicknameService::Service
{
	GENERATED_BODY()
public:
	URegisterNicknameServiceImpl();
	~URegisterNicknameServiceImpl() override;
	void* operator new(size_t size)
	{
		return ::operator new(size);
	}

	static URegisterNicknameServiceImpl* MakeNewObject(const FString& ServerUrl, const FString& Certificate = TEXT("null"), const FString& SslHostName = TEXT("localhost"));
	void Init(FString ServerUrl, const FString& Certificate, const FString& SslHostName);
	
// rpc
	grpc::Status RegisterNickname(grpc::ServerContext* context, const NickName* request, RegisterResponse* response) override;
	
// client
	std::unique_ptr<NicknameService::Stub> ServiceStub;
	
private:
	std::shared_ptr<grpc::Channel> Channel;
};
