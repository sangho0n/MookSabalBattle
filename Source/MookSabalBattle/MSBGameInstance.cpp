// Fill out your copyright notice in the Description page of Project Settings.


#include "MSBGameInstance.h"

IOnlineSubsystem* UMSBGameInstance::OnlineSubsystem;

void UMSBGameInstance::Init()
{
	Super::Init();
	OnlineSubsystem = IOnlineSubsystem::Get();
	DessertMap = FString("/Game/Main/Maps/Map_Dessert");
}

void UMSBGameInstance::EnterGameOnServer(FString ServerIP)
{
	MSB_LOG(Warning, TEXT("ddd"));
	FTimerHandle TimerHandle;
	float DelayInSeconds = 3.0f;
	//auto TimerDelegate = FTimerDelegate::CreateUObject(this, &UMSBGameInstance::EnterGame);
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &UMSBGameInstance::EnterGame);
	
	MSB_LOG(Warning, TEXT("ddd"));
	check(GetWorld() != nullptr);
	check(TimerDelegate.IsBound());
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, DelayInSeconds, false);

	
}

void UMSBGameInstance::EnterGame()
{
	MSB_LOG(Warning, TEXT("ddd"));
	GetWorld()->ServerTravel(DessertMap+TEXT("?listen"), true);

}


void UMSBGameInstance::EnterGameOnClient(FString ServerIP)
{

	auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->ClientTravel(ServerIP+TEXT(":7777"), TRAVEL_Absolute);
	MSB_LOG(Warning, TEXT("ddd"));
}
