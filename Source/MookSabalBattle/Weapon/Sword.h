// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Melee.h"
#include "Sword.generated.h"

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API ASword : public AMelee
{
	GENERATED_BODY()

public:
	ASword();

	virtual void PostInitializeComponents() override;
};
