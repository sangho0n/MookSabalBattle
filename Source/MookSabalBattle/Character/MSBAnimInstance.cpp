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
	OnMontageEnded.AddDynamic(this, &UMSBAnimInstance::OnComboMontageEnded);
	OnOverDeltaOffset.AddDynamic(this, &UMSBAnimInstance::ResetDelta);
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
	CanNextCombo = false;

	OnHitCheck.AddDynamic(OwnedCharacter, &APlayerCharacter::Hit);
	OwnedCharacter->GetCharacterStateComponent()->OnHPIsZero.AddDynamic(this, &UMSBAnimInstance::PlayDeadAnim);
}


void UMSBAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// TODO : below if statement will be deleted after server implemented.
	// for now, just place some other characters in scene for debugging
	if(nullptr != (OwnedCharacter))
	{
		auto Velocity = OwnedCharacter->GetVelocity(); // speed vector in WS
		auto state = OwnedCharacter->GetCharacterStateComponent();
		auto actorRotation = OwnedCharacter->GetActorRotation();
		auto controlRotation = OwnedCharacter->GetControlRotation();
		auto deltaRotation = controlRotation - actorRotation;
		auto actorForward = OwnedCharacter->GetActorForwardVector();
		
		CurrentPawnSpeed = Velocity.Size();
		bInAir = OwnedCharacter->GetMovementComponent()->IsFalling();
		CurrentMode = state->GetCurrentMode();
		bIsAttacking = state->IsAttacking();

		if(OwnedCharacter->GetCurrentMode() == CharacterMode::GUN)
		{

			auto interpRotation = FRotator(Pitch, DeltaYaw, 0);
			interpRotation = FMath::RInterpTo(interpRotation, deltaRotation, DeltaSeconds, 15.0f);
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
			
			if(CurrentPawnSpeed < 0.1)
			{
				// use aim offset
				OwnedCharacter->bUseControllerRotationYaw=false;
			}
			else
			{
				// use directional move
				OwnedCharacter->bUseControllerRotationYaw=true;
				Velocity = FVector(Velocity.X, Velocity.Y, 0); Velocity.Normalize();
				if(FVector::CrossProduct(Velocity, actorForward).Z < 0.0f)
					MovingDirection = FMath::RadiansToDegrees(
						FMath::Acos(FVector::DotProduct(Velocity, actorForward)));
				else
					MovingDirection = -FMath::RadiansToDegrees(
						FMath::Acos(FVector::DotProduct(Velocity, actorForward)));
			}
		}

		//MSB_LOG(Warning,TEXT("yaw %f"), DeltaYaw);
	
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
	RandomMeleeIdx = FMath::RandRange(0, 1);

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

void UMSBAnimInstance::OnComboMontageEnded(UAnimMontage* montage, bool bInterrupted)
{
	auto state = OwnedCharacter->GetCharacterStateComponent();

	state->SetIsAttacking(false);
	CanNextCombo = false;
	CurrentCombo = 0;
	Cast<ALocalPlayerController>(OwnedCharacter->GetController())->OnAttackStop();
	MSB_LOG(Warning,TEXT("montage end : %d"), bInterrupted);
}

void UMSBAnimInstance::AnimNotify_HitCheck()
{
	// detect collision
	OnHitCheck.Broadcast(CurrentCombo);
}


void UMSBAnimInstance::AnimNotify_NextComboCheck()
{
	auto character = Cast<APlayerCharacter>(OwnedCharacter);
	auto state = character->GetCharacterStateComponent();
	auto controller = Cast<ALocalPlayerController>(character->GetController());

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
	MSB_LOG(Warning, TEXT("swing end"));
	auto character = Cast<APlayerCharacter>(OwnedCharacter);
	auto state = character->GetCharacterStateComponent();
	state->SetIsAttacking(false);
}

void UMSBAnimInstance::ResetDelta()
{
	DeltaYaw = 0;
	// set character forward vector to be equal controlling forward vector
	auto controlRotation = OwnedCharacter->GetControlRotation().Vector();
	auto controlRotOnXY = FRotator(FVector(controlRotation.X, controlRotation.Y, 0).Rotation());
	OwnedCharacter->SetActorRotation(controlRotOnXY);
}

void UMSBAnimInstance::PlayDeadAnim()
{
	RandomDeadIdx2 =FMath::RandRange(0,2);
	RandomDeadIdx3 =FMath::RandRange(0,3);
	bIsDead = true;
}

void UMSBAnimInstance::SetMeleeType(AMelee* NewMelee)
{
	CurrentMelee = NewMelee;
	if(CurrentMelee->IsA(APick::StaticClass()))
	{
		SwingPlayRate = 0.6f;
	}
	else if(CurrentMelee->IsA(AAxe::StaticClass()))
	{
		SwingPlayRate = 1.0f;
	}
	else if(CurrentMelee->IsA(ASword::StaticClass()))
	{
		SwingPlayRate = 1.7f;
	}
}

