// Fill out your copyright notice in the Description page of Project Settings.


#include "MSBGameInstance.h"

#include "UI/LobbyUI.h"

IOnlineSubsystem* UMSBGameInstance::OnlineSubsystem;

void UMSBGameInstance::Init()
{
	Super::Init();
	OnlineSubsystem = IOnlineSubsystem::Get();
	DessertMap = FString("/Game/Main/Maps/Map_Dessert");
}

void UMSBGameInstance::EnterGameOnServer(FString ServerIP)
{
	// below code should be deleted when deploying
	if(!ULobbyUI::bIsHost) return;
	
	FTimerHandle TimerHandle;
	float DelayInSeconds = 2.0f;
	auto TimerDelegate = FTimerDelegate::CreateUObject(this, &UMSBGameInstance::EnterGame);
	TimerDelegate.BindUObject(this, &UMSBGameInstance::EnterGame);
	
	MSB_LOG(Warning, TEXT("ddd"));
	check(GetWorld() != nullptr);
	check(TimerDelegate.IsBound());
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, DelayInSeconds, false);

}

void UMSBGameInstance::EnterGame()
{
	MSB_LOG(Warning, TEXT("try enter game"));
	GetWorld()->ServerTravel(DessertMap+TEXT("?listen"), true);
}

void UMSBGameInstance::EnterGameOnClient(FString ServerIP)
{
	// below code should be deleted when deploying
	if(ULobbyUI::bIsHost) return;
	
	auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->ClientTravel(ServerIP, TRAVEL_Absolute);
}
