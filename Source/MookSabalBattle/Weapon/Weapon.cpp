// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
	SM_Weapon = CreateDefaultSubobject<UStaticMeshComponent>("DefaultWeapon");
	Collider = CreateDefaultSubobject<UBoxComponent>("Collider");
	RootComponent = SM_Weapon;
	Collider->SetupAttachment(RootComponent);
	SM_Weapon->SetRelativeLocation(FVector(0, 0, 100));

	OffsetFromLand = FVector(0.0f, 0.0f, 10.0f);
	bIsPossessed = false;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	SM_Weapon->SetSimulatePhysics(true);
	SM_Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SM_Weapon->SetRenderCustomDepth(true);
	SM_Weapon->SetCustomDepthStencilValue(OUT_LINE::Weapon);
	bIsPossessed = false;
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	SM_Weapon->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
	SM_Weapon->SetUseCCD(true);
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
			// show equip UI
			Character->OnWeaponStartOverlap_Server(this);
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
			Character->OnWeaponEndOverlap_Server();
		}
	}
}

void AWeapon::Destroyed()
{
	Super::Destroyed();
}

UStaticMeshComponent* AWeapon::ReadyToEquip(APlayerCharacter* Player)
{
	bIsPossessed = true;
	
	// collider config
	Collider->DestroyComponent();
	SM_Weapon->SetSimulatePhysics(false);
	SM_Weapon->SetCollisionProfileName("CharacterOverlap");
	
	// mesh config
	auto Mesh = this->SM_Weapon;
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OnPlayerPicked.Broadcast();

	return Mesh;
}
