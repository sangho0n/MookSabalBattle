// Fill out your copyright notice in the Description page of Project Settings.


#include "Pick.h"

APick::APick()
{
	Collider->InitBoxExtent(FVector(61.0, 42.0, 42.0));
	SM_Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pick"));
	SM_Weapon->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_PICK(TEXT("/Game/MiniSurvivalProps/SM_Pickaye.SM_Pickaye"));
	if(SM_PICK.Succeeded())
	{
		SM_Weapon->SetStaticMesh(SM_PICK.Object);
	}
	SM_Weapon->SetRelativeRotation(FRotator(0.0f, -90.0f, 90.0));
	SM_Weapon->SetRelativeLocation(FVector(-20.0f, 0.0f, 0.0f));
}

void APick::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

