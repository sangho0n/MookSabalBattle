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
	
public:
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void SetupInputComponent() override;
	
private:
	void ForwardBack(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void Jump();
	void MouseVerticalChange(float NewAxisValue);
	void MouseHorizontalChange(float NewAxisValue);
	void Equip();
	void Attack();
	void Reload();
public:
	void AttackStop();
	void InitPlayer();
private:
	UPROPERTY(Replicated)
	bool bEnableInputControl;
	UPROPERTY(Replicated)
	bool bIsPossessingPawnInitialized;
};
