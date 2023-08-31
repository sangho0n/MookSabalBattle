// Fill out your copyright notice in the Description page of Project Settings.


#include "Sword.h"

ASword::ASword()
{
	SM_Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword"));
	RootComponent = SM_Weapon;
	Collider->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SWORD(TEXT("/Game/MiniSurvivalProps/SM_Sword.SM_Sword"));
	if(SM_SWORD.Succeeded())
	{
		SM_Weapon->SetStaticMesh(SM_SWORD.Object);
	}
	SM_Weapon->SetRelativeRotation(FRotator(0.0f, -90.0f, 90.0));
	Collider->InitBoxExtent(FVector(20.0, 60.0, 90.0));
	Collider->SetRelativeLocation(FVector(0, 0, 50.0f));
}

void ASword::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

