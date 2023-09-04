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
	
	OwnedCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	MaxWalkSpeed = OwnedCharacter->GetMovementComponent()->GetMaxSpeed();
	BeforeDirection = OwnedCharacter->GetControlRotation().Yaw;
	DeltaYaw = 0;
	bIsCW = false;
	CurrentCombo = 0;
	CanNextCombo = true;
}


void UMSBAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// TODO : below if statement will be deleted after server implemented.
	// for now, just place some other characters in scene for debugging
	if(nullptr != (OwnedCharacter))
	{
		auto Velocity = OwnedCharacter->GetVelocity(); // speed vector in WS
		CurrentPawnSpeed = Velocity.Size();
	
		auto state = OwnedCharacter->GetCharacterStateComponent();
		bInAir = OwnedCharacter->GetMovementComponent()->IsFalling();
		CurrentMode = state->GetCurrentMode();
		bIsAttacking = state->IsAttacking();
		MovingDirection = CalculateDirection(Velocity, OwnedCharacter->GetActorRotation());
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
	}
}

bool UMSBAnimInstance::SetIntended(bool isIntended)
{
	this->bIsIntended = isIntended;
	return true;
}

void UMSBAnimInstance::PlayComboAnim()
{
	auto state = OwnedCharacter->GetCharacterStateComponent();
	state->SetIsAttacking(true);
	CanNextCombo = false;
	CurrentCombo = 1;
	
	Montage_Play(ComboMontage);
}

void UMSBAnimInstance::PlayMeleeSwing()
{
	RandomMeleeIdx = FMath::RandRange(0, 2);

	auto state = OwnedCharacter->GetCharacterStateComponent();

	state->SetIsAttacking(true);

}


void UMSBAnimInstance::PlayShot()
{
	auto state = OwnedCharacter->GetCharacterStateComponent();

	state->SetIsAttacking(true);
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
	auto state = OwnedCharacter->GetCharacterStateComponent();

	state->SetIsAttacking(false);
	CanNextCombo = true;
	CurrentCombo = 0;
	Cast<ALocalPlayerController>(OwnedCharacter->GetController())->OnAttackStop();
	MSB_LOG(Warning,TEXT("montage end"));
}

void UMSBAnimInstance::AnimNotify_HitCheck()
{
	// detect collision
	OnHitCheck.Broadcast();
}


void UMSBAnimInstance::AnimNotify_NextComboCheck()
{
	auto character = Cast<APlayerCharacter>(OwnedCharacter);
	auto state = character->GetCharacterStateComponent();
	auto controller = Cast<ALocalPlayerController>(character->GetController());
	controller->OnAttackStop();

	if(NextComboInputOn)
	{
		JumpToNextSection();
		NextComboInputOn = false;
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
	MSB_LOG(Warning, TEXT("asdfsdfasdfasdfasdfasd"));
	auto character = Cast<APlayerCharacter>(OwnedCharacter);
	auto state = character->GetCharacterStateComponent();
	state->SetIsAttacking(false);
}

