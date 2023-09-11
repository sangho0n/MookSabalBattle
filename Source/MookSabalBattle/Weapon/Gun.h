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

	UPROPERTY(BlueprintReadWrite)
	int32 Bullets;
	UPROPERTY(EditAnywhere)
	float GunAttackLength;
	UPROPERTY(EditAnywhere)
	float GunAttackSpreadAngle;

private:
	FName MuzzleSocket;
	FVector MuzzlePosInLocal;
public:
	FVector GetMuzzleLocationInWS();

	void FireParticleOnMuzzle();

private:
	UParticleSystem* FireParticle;
};
