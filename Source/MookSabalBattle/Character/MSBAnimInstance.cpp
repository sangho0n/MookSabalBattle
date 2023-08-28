// Fill out your copyright notice in the Description page of Project Settings.


#include "MSBAnimInstance.h"

#include "PlayerCharacter.h"

UMSBAnimInstance::UMSBAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	Direction = 0.0f;
	bInAir = false;
	bIsIntended = false;
	CurrentMode = CharacterMode::NON_EQUIPPED;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> COMBO_MONTAGE(TEXT("/Game/Main/Animation/Humanoid/NonEquip/AM_NonEqip_Attack.AM_NonEqip_Attack"));
	if(COMBO_MONTAGE.Succeeded())
	{
		this->ComboMontage = COMBO_MONTAGE.Object;
	}
}

void UMSBAnimInstance::NativeBeginPlay()
{
	MaxWalkSpeed = TryGetPawnOwner()->GetMovementComponent()->GetMaxSpeed();
}


void UMSBAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Owner = TryGetPawnOwner();
	if(::IsValid(Owner))
	{		
		auto Velocity = Owner->GetVelocity(); // speed vector in WS
		CurrentPawnSpeed = Velocity.Size();
		
		if(Owner->IsA(APlayerCharacter::StaticClass()))
		{
			auto character = Cast<APlayerCharacter>(Owner);
			auto state = character->GetCharacterStateComponent();
			bInAir = Owner->GetMovementComponent()->IsFalling();
			CurrentMode = state->GetCurrentMode();
			bIsAttacking = state->IsAttacking();
			Direction = CalculateDirection(Velocity, Owner->GetActorRotation());

			if(!bInAir) SetIntended(false);
			return;
		}
	}
}

bool UMSBAnimInstance::SetIntended(bool isIntended)
{
	this->bIsIntended = isIntended;
	return true;
}

void UMSBAnimInstance::PlayComboAnim()
{
	if(!Montage_IsPlaying(ComboMontage))
	{
		Montage_Play(ComboMontage);
	}
}

