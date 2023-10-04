// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Components/ActorComponent.h"
#include "CharacterState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHPIsZero);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChanges, float);

UENUM(BlueprintType)
enum class CharacterMode
{
	NON_EQUIPPED,
	GUN,
	MELEE
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MOOKSABALBATTLE_API ACharacterState : public APlayerState
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ACharacterState();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	bool bIsEquipped;

	UPROPERTY(Replicated, EditAnywhere, Category=Status)
	CharacterMode  CurrentMode;

	UPROPERTY(Replicated)
	bool bIsEquippable;

private:
	UPROPERTY(Replicated)
	bool bIsAttacking;

public:
	UPROPERTY(Replicated)
	bool bIsReloading;

	UPROPERTY(Replicated)
	bool bIsDead;

	UPROPERTY(Replicated, EditAnywhere, Category=Status)
	float HP;

	const float MaxHP = 200.0f;

	UPROPERTY(Replicated, EditAnywhere, Category=Status)
	bool bIsRedTeam;

	FOnHPIsZero OnHPIsZero;
	
	FOnHPChanges OnHPChanges;

	void ApplyDamage(float damage);

public:
	void SetAttacking(bool flag);
	bool IsAttacking();
};
