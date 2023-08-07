// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Gun.generated.h"

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API AGun : public AWeapon
{
	GENERATED_BODY()

public:
	AGun();

	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 Bullets;
	UPROPERTY(EditAnywhere)
	float GunAttackLength;
	UPROPERTY(EditAnywhere)
	float GunAttackSpreadAngle;

private:
	FName MuzzleSocket;
	FVector MuzzlePosInLocal;

	UFUNCTION(Server, Unreliable)
	void FireParticleOnMuzzle_Server();
	UFUNCTION(NetMulticast, Unreliable) 
	void FireParticleOnMuzzle_Multicast();

private:
	UParticleSystem* FireParticle;

public:
	FPointDamageEvent Hit(APlayerCharacter* Causer);

	TSubclassOf<UCameraShakeBase> CamShakeShoot;

	bool CanFire(APlayerCharacter* Causer);

	UFUNCTION()
	void FillBullets();

private:
	UFUNCTION(Server, Reliable)
	void MulticastBulltes(int32 ClientBullets);

public:
	int32 GetBulletCount(){return Bullets;}
};
