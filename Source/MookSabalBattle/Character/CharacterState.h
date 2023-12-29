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
	virtual void Reset() override;

	bool bIsEquipped;

	UPROPERTY(EditAnywhere, Category=Status)
	CharacterMode  CurrentMode;

private:
	UPROPERTY(VisibleAnywhere, Category=Status)
	bool bIsEquippable;

	UPROPERTY(VisibleAnywhere, Category=Status)
	bool bIsAttacking;
	
	UPROPERTY(VisibleAnywhere, Category=Status)
	bool bIsReloading;

	UPROPERTY(VisibleAnywhere, Category=Status)
	bool bIsDead;
	
	UPROPERTY(EditAnywhere, Category=Status)
	float HP;
public:
	const float MaxHP = 200.0f;

private:
	UPROPERTY(Replicated, EditAnywhere, Category=Status)
	bool bIsRedTeam;

public:
	UPROPERTY(Replicated, EditAnywhere, Category=Status)
	int32 KillCount;

	UPROPERTY(Replicated, EditAnywhere, Category=Status)
	int32 DeathCount;

	UPROPERTY(VisibleAnywhere, Category=Status)
	FString NickName;
	
	FOnHPIsZero OnHPIsZero;
	
	FOnHPChanges OnHPChanges;

	UFUNCTION()
	void ApplyDamage(float damage, AActor* Causer);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDamage(float damage);

public:
	void SetAttacking(bool flag);
	void SetEquippable(bool flag);
	void SetHP(float flag);
	void SetTeam(bool isRedTeam){bIsRedTeam = isRedTeam;}
	void SetReload(bool flag){bIsReloading = flag;}
	void SetDead(bool flag){bIsDead = flag;}
	
	bool IsAttacking();
	bool IsEquippable();
	float GetHP();
	bool IsRedTeam(){return bIsRedTeam;}
	bool IsReloading(){return bIsReloading;}
	bool IsDead(){return bIsDead;}

};
