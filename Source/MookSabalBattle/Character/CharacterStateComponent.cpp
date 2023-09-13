// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStateComponent.h"

// Sets default values for this component's properties
UCharacterStateComponent::UCharacterStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCharacterStateComponent::BeginPlay()
{
	Super::BeginPlay();

	HP = MaxHP;
	bIsAttacking = false;
}


// Called every frame
void UCharacterStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCharacterStateComponent::ApplyDamage(float damage)
{
	auto candidateHP = HP - damage;
	HP = candidateHP > 0.0 ? candidateHP : 0.0;
	OnHPChanges.Broadcast(HP);
	MSB_LOG(Warning, TEXT("curr hp : %f"), HP);
	if(HP < KINDA_SMALL_NUMBER)
	{
		HP = 0.0f;
		OnHPIsZero.Broadcast();
	}
}

