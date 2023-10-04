// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "GameFramework/Actor.h"
#include "MookSabalBattle/Character/PlayerCharacter.h"
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
	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, Category=Collision)
	UBoxComponent* Collider;

	UPROPERTY(VisibleAnywhere, Category=Weapon)
	UStaticMeshComponent* SM_Weapon;

	UPROPERTY(VisibleAnywhere, Category=Character)
	APlayerCharacter* LocalPlayer;

	FVector OffsetFromLand;

	virtual void Destroyed() override;
public:	

	UFUNCTION()
	virtual void OnCharacterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void OnCharacterEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UStaticMeshComponent* ReadyToEquip(APlayerCharacter* Player);

	void AfterEquip();

	float Damage;

	bool bIsPossessed;
};
