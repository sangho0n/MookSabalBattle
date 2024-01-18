#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "gRPCSubsystem/Public/RegisterNicknameGrpcSubsystem.h"
#include <stdexcept>
#include "NullSessionSubsystem.generated.h"

DECLARE_DELEGATE_TwoParams(FOnSessionSearchCompleteWithResults, TArray<FOnlineSessionSearchResult>& SessionSearchResults, bool bSucceed);
DECLARE_DELEGATE(FOnInvalidNickname);

class CannotGetSessionInterfaceException : public std::runtime_error
{
public:
    CannotGetSessionInterfaceException(const std::string& message) : std::runtime_error(message){}
};

class CannotGetOnlineSubsystemException : public std::runtime_error
{
public:
    CannotGetOnlineSubsystemException(const std::string& message) : std::runtime_error(message){}
};

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API UNullSessionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UNullSessionSubsystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    
    void HostGame(FString& NickName, int32 MaxPlayerCount, bool bUseLan);
    void TryFindSession(bool bUseLan);
    void JoinSession(const FString& NickName, TWeakPtr<FOnlineSessionSearchResult> SelectedSession);

    int32 MaxPlayer;
    FString LocalPlayerNickName;
    FString TravelURL;
    FString DessertMap;

    FOnSessionSearchCompleteWithResults OnSessionSearchCompleteWithResults;
    FOnInvalidNickname OnInvalidNickname;

private:
    // Session callbacks
    void OnSessionCreate(FName SessionName, bool bWasSucceed);
    void OnFindSessionComplete(bool bSucceed);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type SessionType);

    // Helper functions
    UFUNCTION()
    void CheckNicknameDuplicity(bool bCanJoin);
    void GetSubsystemAndSessionInterface();

    // Session data
    IOnlineSubsystem* OnlineSubsystem;
    IOnlineSession* SessionInterface;
    TSharedPtr<FOnlineSessionSearch> LastSearchResult;
    FName LastJoinedSessionName;

    // gRPC
    URegisterNicknameGrpcSubsystem* gRPCSubsystem;
};
