// Fill out your copyright notice in the Description page of Project Settings.


#include "MSBAnimInstance.h"

#include "LocalPlayerController.h"
#include "PlayerCharacter.h"
#include "MookSabalBattle/Weapon/Axe.h"
#include "MookSabalBattle/Weapon/Pick.h"
#include "MookSabalBattle/Weapon/Sword.h"

UMSBAnimInstance::UMSBAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	MovingDirection = 0.0f;
	bInAir = false;
	bIsIntended = false;
	bIsDead = false;
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
	OnMontageEnded.RemoveAll(this);
	OnOverDeltaOffset.RemoveAll(this);
	OnMontageEnded.AddUniqueDynamic(this, &UMSBAnimInstance::OnComboMontageEnded);
	OnOverDeltaOffset.AddUniqueDynamic(this, &UMSBAnimInstance::ResetDelta);
}


void UMSBAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	OwnedCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	MaxWalkSpeed = OwnedCharacter->GetMovementComponent()->GetMaxSpeed();
	DeltaYaw = 0;
	Pitch = 0;
	bIsCW = false;
	bIsDead = false;
	CurrentCombo = 0;

	OnHitCheck.AddDynamic(OwnedCharacter, &APlayerCharacter::Hit);
}


void UMSBAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(!IsValid(OwnedCharacter)) return;
	if(!IsValid(OwnedCharacter->GetCharacterStateComponent())) return;

	auto Velocity = OwnedCharacter->GetVelocity(); // speed vector in WS
	auto state = OwnedCharacter->GetCharacterStateComponent();
	
	CurrentPawnSpeed = Velocity.Size();
	bInAir = OwnedCharacter->GetMovementComponent()->IsFalling();
	CurrentMode = state->CurrentMode;
	bIsAttacking = state->IsAttacking();
	bIsReload = state->IsReloading();
	bIsDead = state->IsDead();

	if(nullptr != OwnedCharacter->CurrentWeapon && OwnedCharacter->CurrentWeapon->IsA(AMelee::StaticClass()))
	{
		CurrentMelee = Cast<AMelee>(OwnedCharacter->CurrentWeapon);
		SwingPlayRate = CurrentMelee->AttackSpeed;
	}

	if(CurrentMode == CharacterMode::GUN)
	{
		ActorForwardVector = OwnedCharacter->GetActorForwardVector();
		
		if(CurrentPawnSpeed < 0.1)
		{
			// use aim offset
			OwnedCharacter->bUseControllerRotationYaw=false;

			ControlRotation = OwnedCharacter->ControlRotation;
			ActorRotation = OwnedCharacter->ActorRotation;
			auto interpRotation = FRotator(Pitch, DeltaYaw, 0);
			interpRotation = FMath::RInterpTo(interpRotation, ControlRotation - ActorRotation, DeltaSeconds, 15.0f);
			DeltaYaw = FMath::Clamp(interpRotation.Yaw, -180.0f, 180.0f);
			Pitch = FMath::ClampAngle(interpRotation.Pitch, -90.0f, 90.0f);

			if(DeltaYaw > 90.0f)
			{
				bIsCW = true;
				OnOverDeltaOffset.Broadcast();
			}
			else if(DeltaYaw < -90.0f)
			{
				bIsCW = false;
				OnOverDeltaOffset.Broadcast();
			}
		}
		else
		{
			// use directional move
			OwnedCharacter->bUseControllerRotationYaw=true;
			Velocity = FVector(Velocity.X, Velocity.Y, 0); Velocity.Normalize();
			if(FVector::CrossProduct(Velocity, ActorForwardVector).Z < 0.0f)
				MovingDirection = FMath::RadiansToDegrees(
					FMath::Acos(FVector::DotProduct(Velocity, ActorForwardVector)));
			else
				MovingDirection = -FMath::RadiansToDegrees(
					FMath::Acos(FVector::DotProduct(Velocity, ActorForwardVector)));
		}
	}

	if(!bInAir) SetIntended(false);
}

bool UMSBAnimInstance::SetIntended(bool isIntended)
{
	this->bIsIntended = isIntended;
	return true;
}

void UMSBAnimInstance::PlayComboAnim()
{
	CurrentCombo = 1;
	NextComboInputOn = false;
	
	Montage_Play(ComboMontage);
}

void UMSBAnimInstance::PlayRandomMeleeSwing()
{
	RandomMeleeIdx = FMath::RandRange(0, 1);
}

void UMSBAnimInstance::JumpToNextSection()
{
	auto text = GetNextComboSectionName();
	Montage_JumpToSection(text, ComboMontage);
}

void UMSBAnimInstance::OnComboMontageEnded(UAnimMontage* montage, bool bInterrupted)
{
	if(bInterrupted) return;
	CurrentCombo = 0;
	NextComboInputOn = false;
	OnAttackEnd.Broadcast();
	
	if(!OwnedCharacter->HasAuthority()) return;
	Cast<ALocalPlayerController>(OwnedCharacter->GetController())->AttackStop();
}

void UMSBAnimInstance::AnimNotify_HitCheck()
{
	// detect collision
	OnHitCheck.Broadcast(CurrentCombo);
}

void UMSBAnimInstance::AnimNotify_NextComboCheck()
{
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

void UMSBAnimInstance::ReloadEnd()
{
	OnReloadAnimEnd.Broadcast();
}

void UMSBAnimInstance::SwingEnd()
{
	OnAttackEnd.Broadcast();
}

void UMSBAnimInstance::ResetDelta()
{
	DeltaYaw = 0;
	// set character forward vector to be equal controlling forward vector
	auto ControlRotOnXY = FRotator(FVector(ControlRotation.Vector().X, ControlRotation.Vector().Y, 0).Rotation());
	OwnedCharacter->SetActorRotation(ControlRotOnXY);
}

void UMSBAnimInstance::PlayRandomDeadAnim()
{
	RandomDeadIdx2 =FMath::RandRange(0,1);
	RandomDeadIdx3 =FMath::RandRange(0,2);
}
