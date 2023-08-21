// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS(Abstract)
class MOOKSABALBATTLE_API AWeapon : public AActor 
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category=Collision)
	UBoxComponent* Collider;

	UPROPERTY(VisibleAnywhere, Category=Weapon)
	UStaticMeshComponent* SM_Weapon;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
