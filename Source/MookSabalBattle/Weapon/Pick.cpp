// Fill out your copyright notice in the Description page of Project Settings.


#include "Pick.h"

APick::APick()
{
	SM_Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pick"));
	RootComponent = SM_Weapon;
	Collider->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_PICK(TEXT("/Game/MiniSurvivalProps/SM_Pickaye.SM_Pickaye"));
	if(SM_PICK.Succeeded())
	{
		SM_Weapon->SetStaticMesh(SM_PICK.Object);
	}
	SM_Weapon->SetRelativeLocation(OffsetFromLand);
	SM_Weapon->SetRelativeRotation(FRotator(0.0f, -90.0f, 90.0));
	Collider->InitBoxExtent(FVector(57.0, 60.0, 64.0));
	Collider->SetRelativeLocation(FVector(0.0f, 0.0f, 20.0f));
}

void APick::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	AttackCapsuleColliderHalfHeight = 60.0f;
	AttackCapsuleColliderRadius = 50.0f;
	AttackSpeed = 0.6f;
	Damage = 35.0f;
}

