// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Melee.h"
#include "Pick.generated.h"

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API APick : public AMelee
{
	GENERATED_BODY()

public:
	APick();

	virtual void PostInitializeComponents() override;
};
