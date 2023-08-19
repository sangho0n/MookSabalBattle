// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

AGun::AGun() : Super()
{
	SK_Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	SK_Weapon->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_MESH(TEXT("Game/MiniSurvivalProps/SM_AssaultRifle.SM_AssaultRifle"));
	if(SK_MESH.Succeeded())
	{
		SK_Weapon->SetSkeletalMesh(SK_MESH.Object);
	}
}

void AGun::BeginPlay()
{
	Super::BeginPlay();
}
