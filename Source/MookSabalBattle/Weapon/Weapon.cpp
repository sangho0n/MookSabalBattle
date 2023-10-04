// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
	SM_Weapon = CreateDefaultSubobject<UStaticMeshComponent>("DefaultWeapon");
	Collider = CreateDefaultSubobject<UBoxComponent>("Collider");
	RootComponent = SM_Weapon;
	Collider->SetupAttachment(RootComponent);

	LocalPlayer = nullptr;
	SM_Weapon->SetRelativeLocation(FVector(0, 0, 100));

	OffsetFromLand = FVector(0.0f, 0.0f, 10.0f);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	SM_Weapon->SetSimulatePhysics(true);
	SM_Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	SM_Weapon->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
	Collider->SetCollisionProfileName(TEXT("Weapon"));
	Collider->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnCharacterBeginOverlap);
	Collider->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnCharacterEndOverlap);
}


void AWeapon::OnCharacterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		auto Character = Cast<APlayerCharacter>(OtherActor);
		if(Character->IsLocallyControlled())
		{
			MSB_LOG(Warning, TEXT("begin"));
			MSB_LOG(Warning, TEXT("dd %s"), *SM_Weapon->GetName());
			// show equip UI
			Character->OnWeaponStartOverlap(this);
		}
	}
}

void AWeapon::OnCharacterEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		auto Character = Cast<APlayerCharacter>(OtherActor);
		if(Character->IsLocallyControlled())
		{
			// hide equip UI
			MSB_LOG(Warning, TEXT("end"));
			Character->OnWeaponEndOverlap();
		}
	}
}

void AWeapon::Destroyed()
{
	if(nullptr != LocalPlayer)
		OnCharacterEndOverlap(nullptr, LocalPlayer, nullptr, -1);
	Super::Destroyed();
}

UStaticMeshComponent* AWeapon::ReadyToEquip()
{
	// collider config
	Collider->DestroyComponent();
	SM_Weapon->SetSimulatePhysics(false);
	SM_Weapon->SetCollisionProfileName("CharacterOverlap");
	
	// mesh config
	auto mesh = this->SM_Weapon;
	mesh->SetSimulatePhysics(false);
	mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	return mesh;
}
