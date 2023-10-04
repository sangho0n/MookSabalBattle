// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterState.h"

#include "Containers/StackTracker.h"
#include "Net/UnrealNetwork.h"
#include "Serialization/ArchiveStackTrace.h"

// Sets default values for this component's properties
ACharacterState::ACharacterState()
{
	CurrentMode = CharacterMode::NON_EQUIPPED;
	bIsEquippable = false;
	bIsAttacking = false;
	bIsReloading = false;
	bIsDead = false;
	bIsEquipped = false;
	bIsRedTeam = false;
}

void ACharacterState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACharacterState, CurrentMode);
	DOREPLIFETIME(ACharacterState, bIsEquippable);
	DOREPLIFETIME(ACharacterState, bIsAttacking);
	DOREPLIFETIME(ACharacterState, bIsReloading);
	DOREPLIFETIME(ACharacterState, bIsDead);
	DOREPLIFETIME(ACharacterState, HP);
	DOREPLIFETIME(ACharacterState, bIsRedTeam);
}


// Called when the game starts
void ACharacterState::BeginPlay()
{
	Super::BeginPlay();

	HP = MaxHP;
	bIsAttacking = false;
	bIsReloading = false;
}

void ACharacterState::ApplyDamage(float damage)
{
	if(!GetOwner()->HasAuthority()) return;
	
	auto candidateHP = HP - damage;
	HP = candidateHP > 0.0f ? candidateHP : 0.0f;
	MSB_LOG(Warning, TEXT("curr hp : %f"), HP);
	if(HP < 0.01)
	{
		HP = 0.0f;
		MSB_LOG(Warning, TEXT("before die"));
		OnHPIsZero.Broadcast();
	}
	OnHPChanges.Broadcast(HP);
}

void ACharacterState::SetAttacking(bool flag)
{
	bIsAttacking = flag;
}

bool ACharacterState::IsAttacking()
{
	return bIsAttacking;
}

void ACharacterState::SetEquippable(bool flag)
{
	bIsEquippable = flag;
	MSB_LOG(Warning, TEXT("equippable set %s"), bIsEquippable?TEXT("true"):TEXT("false"));
}

bool ACharacterState::IsEquippable()
{
	return bIsEquippable;
}

