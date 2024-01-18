// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MookSabalBattle.h"
#include "Engine/GameInstance.h"
#include "MSBGameInstance.generated.h"

DECLARE_DELEGATE(FOnLoading);
DECLARE_DELEGATE(FStopLoading);

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API UMSBGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UMSBGameInstance();

	virtual void Init() override;

	void StartLoading();
	void EndLoading();
	
private:
	TSubclassOf<UUserWidget> LoadingWidgetClass;
	UUserWidget* LoadingWidget;
	
	FOnLoading OnLoading;
	FStopLoading StopLoading;
};
