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
}

void AGun::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Bullets = 0;
	GunAttackLength = 1000.0f;
	GunAttackSpreadAngle = 3.0f;
}

FVector AGun::GetMuzzleLocationInWS()
{
	return SM_Weapon->GetSocketLocation(MuzzleSocket);
}
