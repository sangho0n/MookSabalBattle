// Fill out your copyright notice in the Description page of Project Settings.


#include "Sword.h"

ASword::ASword()
{
	Collider->InitBoxExtent(FVector(76.0, 18.0, 42.0));
	SM_Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword"));
	SM_Weapon->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SWORD(TEXT("/Game/MiniSurvivalProps/SM_Sword.SM_Sword"));
	if(SM_SWORD.Succeeded())
	{
		SM_Weapon->SetStaticMesh(SM_SWORD.Object);
	}
	SM_Weapon->SetRelativeRotation(FRotator(0.0f, -90.0f, 90.0));
	SM_Weapon->SetRelativeLocation(FVector(-45.0f, 0.0f, 0.0f));
}

void ASword::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

