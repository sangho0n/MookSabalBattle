// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

AGun::AGun() : Super()
{
	SM_Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gun"));
	RootComponent = SM_Weapon;
	Collider->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_GUN(TEXT("/Game/MiniSurvivalProps/SM_AssaultRifle.SM_AssaultRifle"));
	if(SM_GUN.Succeeded())
	{
		SM_Weapon->SetStaticMesh(SM_GUN.Object);
	}
	SM_Weapon->SetRelativeLocation(OffsetFromLand);
	SM_Weapon->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0));
	Collider->InitBoxExtent(FVector(45.0, 60.0, 28.0));
	Collider->SetRelativeLocation(FVector(17.0f, 0.0f, 11.0f));

	MuzzleSocket = FName("Muzzle");

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_Particle(TEXT("/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Sparks/P_Sparks_C.P_Sparks_C"));
	if(P_Particle.Succeeded())
	{
		FireParticle = P_Particle.Object;
	}
}

void AGun::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Bullets = 0;
	GunAttackLength = 7000.0f;
	GunAttackSpreadAngle = 3.0f;
}

FVector AGun::GetMuzzleLocationInWS()
{
	return SM_Weapon->GetSocketLocation(MuzzleSocket);
}

void AGun::FireParticleOnMuzzle()
{
	UGameplayStatics::SpawnEmitterAttached(FireParticle,
		SM_Weapon,
		MuzzleSocket,
		FVector(0, 0, 0),
		FRotator(-90.0f, 0, 0),
		FVector(0.2f)
		);
}

