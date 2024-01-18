// Fill out your copyright notice in the Description page of Project Settings.


#include "MSBGameInstance.h"
#include "Blueprint/UserWidget.h"

UMSBGameInstance::UMSBGameInstance()
{
	ConstructorHelpers::FClassFinder<UUserWidget> LOADING(TEXT("/Game/Main/UI/Loading.Loading_C"));
	if(LOADING.Succeeded())
	{
		LoadingWidgetClass = LOADING.Class;
	}
}


void UMSBGameInstance::Init()
{
	Super::Init();

	OnLoading.BindUObject(this, &UMSBGameInstance::StartLoading);
	StopLoading.BindUObject(this, &UMSBGameInstance::EndLoading);
}

void UMSBGameInstance::StartLoading()
{
	if(LoadingWidgetClass->IsValidLowLevel())
		LoadingWidget = CreateWidget(this, LoadingWidgetClass); LoadingWidget->AddToViewport();
}

void UMSBGameInstance::EndLoading()
{
	LoadingWidget->RemoveFromViewport();
	LoadingWidget = nullptr;
}

