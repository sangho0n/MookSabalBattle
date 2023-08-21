// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

AGun::AGun() : Super()
{
	SM_Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gun"));
	SM_Weapon->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_GUN(TEXT("/Game/MiniSurvivalProps/SM_AssaultRifle.SM_AssaultRifle"));
	if(SM_GUN.Succeeded())
	{
		SM_Weapon->SetStaticMesh(SM_GUN.Object);
	}
	SM_Weapon->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0));
	SM_Weapon->SetRelativeLocation(FVector(-20.0f, 0.0f, 0.0f));
}

void AGun::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
