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

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	Collider->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnCharacterBeginOverlap);
	Collider->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnCharacterEndOverlap);
}


void AWeapon::OnCharacterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// show equip UI
	MSB_LOG(Warning, TEXT("begin"));
	MSB_LOG(Warning, TEXT("dd %s"), *SM_Weapon->GetName());
}

void AWeapon::OnCharacterEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// hide equip UI
	MSB_LOG(Warning, TEXT("end"));
}