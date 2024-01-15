#pragma once

#include "gRPCSubsystem/gRPCSubsystem.h"
#include "RegisterNicknameServiceImpl.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RegisterNicknameServiceImpl.h"
#include "grpcpp/server.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformAtomics.h"
#include "Windows/PreWindowsApi.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformAtomics.h"
#endif

#include "RegisterNicknameGrpcSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClientReceiveMessage, const bool, canJoin);


/**
 * 
 */
UCLASS()
class GRPCSUBSYSTEM_API URegisterNicknameGrpcSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "GrpcNicknameService")
	void StartListen(FString PlayerNickname);

	UFUNCTION(BlueprintCallable, Category = "GrpcNicknameService")
	void RequestRegister(const FString NickFStr, const FString ServerUrl, const FString Certificate = TEXT("null"), const FString SslHostName = TEXT("localhost"));

	UPROPERTY(BlueprintAssignable, Category = "GrpcNicknameService")
	FOnClientReceiveMessage OnClientReceiveMessage;

private:
	std::unique_ptr<RegisterNicknameServiceImpl> RegisterNicknameService;

	std::shared_ptr<NickName> Nickname;
	std::shared_ptr<RegisterResponse> Response;
	std::unique_ptr<grpc::Server> gRPCServer;
};
