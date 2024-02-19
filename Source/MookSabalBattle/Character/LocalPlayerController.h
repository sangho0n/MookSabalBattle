// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "LocalPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API ALocalPlayerController : public APlayerController
{
	GENERATED_BODY()

	ALocalPlayerController();
	
public:
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void SetupInputComponent() override;
	
private:
	void Move(const FInputActionValue& Value);
	void Jump();
	void Look(const FInputActionValue& Value);
	void Equip();
	void Attack();
	void Reload();
public:
	void AttackStop();
	void InitPlayer();
private:
UPROPERTY(Replicated)
	bool bIsPossessingPawnInitialized;

public:
	UFUNCTION(Client, Reliable)
	void DisableInputEachCient(APlayerController* Controller);

private:
	TObjectPtr<UInputMappingContext> IMC_Normal;
	TObjectPtr<UInputAction> IA_DirMove;
	TObjectPtr<UInputAction> IA_Look;
	TObjectPtr<UInputAction> IA_Attack;
	TObjectPtr<UInputAction> IA_Reload;
	TObjectPtr<UInputAction> IA_Jump;
	TObjectPtr<UInputAction> IA_Equip;
};
