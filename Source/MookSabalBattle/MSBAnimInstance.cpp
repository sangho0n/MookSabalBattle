// Fill out your copyright notice in the Description page of Project Settings.


#include "MSBAnimInstance.h"

#include "PlayerCharacter.h"

UMSBAnimInstance::UMSBAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	bInAir = false;
	bIsIntended = false;
}

void UMSBAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Owner = TryGetPawnOwner();
	if(::IsValid(Owner))
	{
		auto velocity = Owner->GetVelocity();
		auto speed = velocity.Size();
		MSB_LOG(Warning, TEXT("velocity %s"), *velocity.ToString());
		MSB_LOG(Warning, TEXT("speed %f"), speed);
		CurrentPawnSpeed = speed;
		//CurrentPawnSpeed = Owner->GetVelocity().Size();
		MSB_LOG(Warning, TEXT("current speed : %f"), CurrentPawnSpeed);
		if(Owner->IsA(APlayerCharacter::StaticClass()))
		{
			bInAir = Owner->GetMovementComponent()->IsFalling();
		}
	}
}
