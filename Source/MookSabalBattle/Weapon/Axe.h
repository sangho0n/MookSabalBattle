// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Melee.h"
#include "Axe.generated.h"

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API AAxe : public AMelee
{
	GENERATED_BODY()

public:
	AAxe();

	virtual void PostInitializeComponents() override;
};
