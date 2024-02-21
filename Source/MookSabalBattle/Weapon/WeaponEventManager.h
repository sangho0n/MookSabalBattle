// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "WeaponEventManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnFire, FVector, MuzzlePoint, FVector, TargetPoint, bool, bIsBlocked, bool, bIsHit);
/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API UWeaponEventManager : public UObject
{
	GENERATED_BODY()

	UWeaponEventManager();

	virtual void PostInitProperties() override;
	
public:
	// UGameInstanceSubsystem cannot be designated as the parent class of a Blueprint class,
	// so I decided to inherit from UObject and apply the singleton pattern.
	static TObjectPtr<UWeaponEventManager> GetInstance();

	virtual void BeginDestroy() override;
	
	UPROPERTY(BlueprintAssignable)
	FOnFire OnFire;

	TObjectPtr<UParticleSystem> FireParticle;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
	TObjectPtr<UNiagaraSystem> BulletTrace;

	UFUNCTION()
	void SpawnFireParticle(FVector MuzzlePoint, FVector TargetPoint, bool bIsBlocked, bool bIsHit);

private:
	static TObjectPtr<UWeaponEventManager> Instance;
};
