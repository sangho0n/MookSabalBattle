// Fill out your copyright notice in the Description page of Project Settings.


#include "MSBAnimInstance.h"

#include "LocalPlayerController.h"
#include "PlayerCharacter.h"

UMSBAnimInstance::UMSBAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	MovingDirection = 0.0f;
	bInAir = false;
	bIsIntended = false;
	CurrentMode = CharacterMode::NON_EQUIPPED;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> COMBO_MONTAGE(TEXT("/Game/Main/Animation/Humanoid/NonEquip/AM_NonEquip_Attack.AM_NonEquip_Attack"));
	if(COMBO_MONTAGE.Succeeded())
	{
		this->ComboMontage = COMBO_MONTAGE.Object;
	}
}

void UMSBAnimInstance::PostInitProperties()
{
	Super::PostInitProperties();
	OnMontageEnded.AddDynamic(this, &UMSBAnimInstance::OnComboMontageEnded);
}


void UMSBAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	MaxWalkSpeed = TryGetPawnOwner()->GetMovementComponent()->GetMaxSpeed();
	BeforeDirection = TryGetPawnOwner()->GetControlRotation().Yaw;
	DeltaYaw = 0;
	bIsCW = false;
	OwnedPawn = Cast<APlayerCharacter>(TryGetPawnOwner());
	CurrentCombo = 0;
	CanNextCombo = true;
}


void UMSBAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(::IsValid(OwnedPawn))
	{		
		auto Velocity = OwnedPawn->GetVelocity(); // speed vector in WS
		CurrentPawnSpeed = Velocity.Size();
		
		if(OwnedPawn->IsA(APlayerCharacter::StaticClass()))
		{
			auto character = Cast<APlayerCharacter>(OwnedPawn);
			auto state = character->GetCharacterStateComponent();
			bInAir = OwnedPawn->GetMovementComponent()->IsFalling();
			CurrentMode = state->GetCurrentMode();
			bIsAttacking = state->IsAttacking();
			MovingDirection = CalculateDirection(Velocity, OwnedPawn->GetActorRotation());
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
	if(::IsValid(OwnedPawn))
	{		
		if(OwnedPawn->IsA(APlayerCharacter::StaticClass()))
		{
			auto character = Cast<APlayerCharacter>(OwnedPawn);
			auto state = character->GetCharacterStateComponent();
			state->SetIsAttacking(true);
			CanNextCombo = false;
			CurrentCombo = 1;
			
			Montage_Play(ComboMontage);
		}
	}
}

void UMSBAnimInstance::PlayMeleeSwing()
{
	RandomMeleeIdx = FMath::RandRange(0, 2);
	if(::IsValid(OwnedPawn))
	{		
		if(OwnedPawn->IsA(APlayerCharacter::StaticClass()))
		{
			auto character = Cast<APlayerCharacter>(OwnedPawn);
			auto state = character->GetCharacterStateComponent();

			state->SetIsAttacking(true);
		}
	}
}

void UMSBAnimInstance::JumpToNextSection()
{
	auto text = GetNextComboSectionName();
	Montage_JumpToSection(text, ComboMontage);
}


void UMSBAnimInstance::SetBeforeDirection(float NewBeforeDir)
{
	DeltaYaw += NewBeforeDir - BeforeDirection;
	BeforeDirection = NewBeforeDir;
}

void UMSBAnimInstance::OnComboMontageEnded(UAnimMontage* montage, bool bInterrupted)
{
	if(::IsValid(OwnedPawn))
	{		
		if(OwnedPawn->IsA(APlayerCharacter::StaticClass()))
		{
			auto character = Cast<APlayerCharacter>(OwnedPawn);
			auto state = character->GetCharacterStateComponent();

			state->SetIsAttacking(false);
			CanNextCombo = true;
			CurrentCombo = 0;
			Cast<ALocalPlayerController>(character->GetController())->OnAttackStop();
			MSB_LOG(Warning,TEXT("montage end"));
		}
	}
}

void UMSBAnimInstance::AnimNotify_HitCheck()
{
	if(::IsValid(OwnedPawn))
	{		
		if(OwnedPawn->IsA(APlayerCharacter::StaticClass()))
		{
			auto character = Cast<APlayerCharacter>(OwnedPawn);
			auto state = character->GetCharacterStateComponent();

			// detect collision
		}
	}
}


void UMSBAnimInstance::AnimNotify_NextComboCheck()
{
	if(::IsValid(OwnedPawn))
	{		
		if(OwnedPawn->IsA(APlayerCharacter::StaticClass()))
		{
			auto character = Cast<APlayerCharacter>(OwnedPawn);
			auto state = character->GetCharacterStateComponent();
			auto controller = Cast<ALocalPlayerController>(character->GetController());
			controller->OnAttackStop();

			if(NextComboInputOn)
			{
				JumpToNextSection();
				NextComboInputOn = false;
			}
		}
	}
}

FName UMSBAnimInstance::GetNextComboSectionName()
{
	CurrentCombo = FMath::Clamp(CurrentCombo+1, 1, 3);
	auto NextSection = FName(*FString::Printf(TEXT("Combo%d"), CurrentCombo));
	return NextSection;
}

void UMSBAnimInstance::SetSwingEnd()
{
	if(::IsValid(OwnedPawn))
	{		
		if(OwnedPawn->IsA(APlayerCharacter::StaticClass()))
		{
			MSB_LOG(Warning, TEXT("asdfsdfasdfasdfasdfasd"));
			auto character = Cast<APlayerCharacter>(OwnedPawn);
			auto state = character->GetCharacterStateComponent();
			state->SetIsAttacking(false);
		}
	}
}

