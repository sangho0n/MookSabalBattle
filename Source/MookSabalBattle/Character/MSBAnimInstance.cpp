// Fill out your copyright notice in the Description page of Project Settings.


#include "MSBAnimInstance.h"

#include "PlayerCharacter.h"

UMSBAnimInstance::UMSBAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	MovingDirection = 0.0f;
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
	BeforeDirection = TryGetPawnOwner()->GetControlRotation().Yaw;
	DeltaYaw = 0;
	bIsCW = false;
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
			MovingDirection = CalculateDirection(Velocity, Owner->GetActorRotation());
			if(DeltaYaw > 30.0f)
			{
				bIsCW = true;
				DeltaYaw = 0.0f;
			}
			else if (DeltaYaw < -30.0f)
			{
				bIsCW = false;
				DeltaYaw = 0.0f;
			}
			
			MSB_LOG(Warning, TEXT("before yaw %f"), DeltaYaw);
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

void UMSBAnimInstance::SetBeforeDirection(float NewBeforeDir)
{
	DeltaYaw += NewBeforeDir - BeforeDirection;
	BeforeDirection = NewBeforeDir;
}

