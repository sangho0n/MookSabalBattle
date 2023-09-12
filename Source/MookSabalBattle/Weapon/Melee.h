// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Weapon.h"
#include "Melee.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class MOOKSABALBATTLE_API AMelee : public AWeapon
{
	GENERATED_BODY()

public:
	AMelee();

	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere)
	float AttackCapsuleColliderHalfHeight;
	UPROPERTY(EditAnywhere)
	float AttackCapsuleColliderRadius;

	float AttackSpeed;
};
