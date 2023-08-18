// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "GameFramework/PlayerController.h"
#include "LocalPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API ALocalPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	APawn* controllingPawn;

public:
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* InPawn) override;

protected:
	virtual void SetupInputComponent() override;

private:
	void ForwardBack(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void SpacebarPressed();
	void MouseVerticalChange(float NewAxisValue);
	void MouseHorizontalChange(float NewAxisValue);
};
