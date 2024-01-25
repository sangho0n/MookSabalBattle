// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "GameFramework/Actor.h"
#include "MookSabalBattle/Weapon/Weapon.h"
#include "NiagaraComponent.h"
#include "WeaponSpawner.generated.h"

UCLASS()
class MOOKSABALBATTLE_API AWeaponSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponSpawner();

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SpawnRandomWeapon();

private:
	UPROPERTY(Replicated)
	bool bSpawningWeapon;
	UPROPERTY(Replicated)
	bool bSpawnedWeapon;

	UPROPERTY(Replicated)
	TObjectPtr<AWeapon> Weapon;

	TArray<UClass*> WeaponClasses;

	UNiagaraComponent* SpawnNiagaraComponent;

	UFUNCTION()
	void UnPossessWeapon();
};
