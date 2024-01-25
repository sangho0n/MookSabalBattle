// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSpawner.h"

#include "MookSabalBattle/Weapon/Axe.h"
#include "MookSabalBattle/Weapon/Gun.h"
#include "MookSabalBattle/Weapon/Pick.h"
#include "MookSabalBattle/Weapon/Sword.h"
#include "Engine/TimerHandle.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeaponSpawner::AWeaponSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpawnerEffect"));
	SpawnNiagaraComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemAsset(TEXT("/Game/sA_PickupSet_1/Fx/NiagaraSystems/NS_Pickup_1.NS_Pickup_1"));
	if(NiagaraSystemAsset.Succeeded())
	{
		SpawnNiagaraComponent->SetAsset(NiagaraSystemAsset.Object);
	}
}

void AWeaponSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponSpawner, bSpawnedWeapon);
	DOREPLIFETIME(AWeaponSpawner, bSpawningWeapon);
	DOREPLIFETIME(AWeaponSpawner, Weapon);
}


void AWeaponSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	bSpawningWeapon = false;
	bSpawnedWeapon = false;

	WeaponClasses.Add(AAxe::StaticClass());
	WeaponClasses.Add(AGun::StaticClass());
	WeaponClasses.Add(APick::StaticClass());
	WeaponClasses.Add(ASword::StaticClass());

	if(SpawnNiagaraComponent)
	{
		SpawnNiagaraComponent->Activate();
	}
}

void AWeaponSpawner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!HasAuthority()) return;

	if(!bSpawnedWeapon && !bSpawningWeapon)
	{
		bSpawningWeapon= true;
		// Timer for spawning weapons
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AWeaponSpawner::SpawnRandomWeapon,
			FMath::RandRange(1.0f, 5.0f), false);
	}
}

void AWeaponSpawner::SpawnRandomWeapon()
{
	bSpawningWeapon = false;
	// Spawn weapon randomly
	int32 RandomWeaponIndex = FMath::RandRange(0, WeaponClasses.Num() - 1);
	UClass* WeaponClass = WeaponClasses[RandomWeaponIndex];
	Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, GetActorTransform());
	Weapon->OnPlayerPicked.AddDynamic(this, &AWeaponSpawner::UnPossessWeapon);

	bSpawnedWeapon = true;
}

void AWeaponSpawner::UnPossessWeapon()
{
	bSpawnedWeapon = false;
}
