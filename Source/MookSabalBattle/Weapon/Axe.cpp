// Fill out your copyright notice in the Description page of Project Settings.


#include "Axe.h"

AAxe::AAxe()
{
	SM_Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Axe"));
	RootComponent = SM_Weapon;
	Collider->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_AXE(TEXT("/Game/MiniSurvivalProps/SM_Axe.SM_Axe"));
	if(SM_AXE.Succeeded())
	{
		SM_Weapon->SetStaticMesh(SM_AXE.Object);
	}
	SM_Weapon->SetRelativeLocation(OffsetFromLand);
	SM_Weapon->SetRelativeRotation(FRotator(0.0f, -90.0f, 90.0));
	Collider->InitBoxExtent(FVector(25.0, 60.0, 42.0));
	Collider->SetRelativeLocation(FVector(0.0f, 0.0f, 20.0f));
}

void AAxe::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	AttackCapsuleColliderHalfHeight = 35.0f;
	AttackCapsuleColliderRadius = 34.0f;
}

