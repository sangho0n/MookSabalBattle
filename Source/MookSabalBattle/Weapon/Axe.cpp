// Fill out your copyright notice in the Description page of Project Settings.


#include "Axe.h"

AAxe::AAxe()
{
	Collider->InitBoxExtent(FVector(45.0, 25.0, 42.0));
	SM_Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Axe"));
	SM_Weapon->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_AXE(TEXT("/Game/MiniSurvivalProps/SM_Axe.SM_Axe"));
	if(SM_AXE.Succeeded())
	{
		SM_Weapon->SetStaticMesh(SM_AXE.Object);
	}
	SM_Weapon->SetRelativeRotation(FRotator(0.0f, -90.0f, 90.0));
	SM_Weapon->SetRelativeLocation(FVector(-20.0f, 0.0f, 0.0f));
}

void AAxe::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

