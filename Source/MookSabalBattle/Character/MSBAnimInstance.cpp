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
			bInAir = Owner->GetMovementComponent()->IsFalling();
			auto character = Cast<APlayerCharacter>(Owner);
			auto state = character->GetCharacterStateComponent();
			CurrentMode = state->GetCurrentMode();
			bIsAttacking = state->IsAttacking();
			
			Direction = CalculateDirection(Velocity, Owner->GetActorRotation());
			return;
		}
	}
}
