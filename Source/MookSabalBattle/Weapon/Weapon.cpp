// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
	SM_Weapon = CreateDefaultSubobject<UStaticMeshComponent>("DefaultWeapon");
	Collider = CreateDefaultSubobject<UBoxComponent>("Collider");
	RootComponent = Collider;
	Collider->InitBoxExtent(FVector(51.0, 30.0, 42.0));
	
	SM_Weapon->SetCollisionProfileName(TEXT("NoCollision"));
	Collider->SetCollisionProfileName(TEXT("CharacterOverlap"));
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
