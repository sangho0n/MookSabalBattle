// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
	SK_Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("DefaultWeapon");
	Collider = CreateDefaultSubobject<UPrimitiveComponent>("Collider");
	RootComponent = Collider;
	
	SK_Weapon->SetCollisionProfileName(TEXT("NoCollision"));
	Collider->SetCollisionProfileName(TEXT("Weapon"));
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	Collider->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::AWeapon::OnBeginOverlap);
	Collider->OnComponentEndOverlap.AddDynamic(this, &AWeapon::AWeapon::OnEndOverlap);
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// show equip UI
}

void AWeapon::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// hide equip UI
}
