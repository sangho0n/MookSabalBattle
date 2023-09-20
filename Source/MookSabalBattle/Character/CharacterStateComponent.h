// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Components/ActorComponent.h"
#include "CharacterStateComponent.generated.h"

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
class MOOKSABALBATTLE_API UCharacterStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool bIsEquipped;

	UPROPERTY(EditAnywhere, Category=Status)
	CharacterMode CurrentMode;

	bool bIsEquippable;

	bool bIsAttacking;

	bool bIsReloading;

	bool bIsDead;

	UPROPERTY(EditAnywhere, Category=Status)
	float HP;

	const float MaxHP = 200.0f;

	FString NickName;

	FOnHPIsZero OnHPIsZero;
	
	FOnHPChanges OnHPChanges;

	void ApplyDamage(float damage);
};
