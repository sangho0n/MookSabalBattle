// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalPlayerController.h"

#include "MSBAnimInstance.h"
#include "PlayerCharacter.h"

void ALocalPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ALocalPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	controllingPawn = InPawn;
	bEnableInputControl = true;
}

void ALocalPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &ALocalPlayerController::ForwardBack);
	InputComponent->BindAxis("MoveRight", this, &ALocalPlayerController::LeftRight);
	InputComponent->BindAction("SpaceBar", IE_Pressed, this, &ALocalPlayerController::SpacebarPressed);
	InputComponent->BindAxis("LookUp", this, &ALocalPlayerController::MouseVerticalChange);
	InputComponent->BindAxis("Turn", this, &ALocalPlayerController::MouseHorizontalChange);
	InputComponent->BindAction("Equip", IE_Pressed, this, &ALocalPlayerController::FKeyPressed);
	InputComponent->BindAction("Attack", IE_Pressed, this, &ALocalPlayerController::OnAttack);
	InputComponent->BindAction("Attack", IE_Released, this, &ALocalPlayerController::OnAttackStop);
	InputComponent->BindAction("Reload", IE_Released, this, &ALocalPlayerController::OnReload);
}

void ALocalPlayerController::ForwardBack(float NewAxisValue)
{
	if(!bEnableInputControl) return;
	
	if(controllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(controllingPawn);
		character->ForwardBack(NewAxisValue);
	}
}

void ALocalPlayerController::LeftRight(float NewAxisValue)
{
	if(!bEnableInputControl) return;
	
	if(controllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(controllingPawn);
		character->LeftRight(NewAxisValue);
	}
}

void ALocalPlayerController::SpacebarPressed()
{
	if(!bEnableInputControl) return;
	
	if(controllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(controllingPawn);
		character->Jump();
	}
}

void ALocalPlayerController::MouseVerticalChange(float NewAxisValue)
{
	if(!bEnableInputControl) return;
	
	if(controllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(controllingPawn);
		character->LookUp(NewAxisValue);
	}
	AddPitchInput(NewAxisValue);
}

void ALocalPlayerController::MouseHorizontalChange(float NewAxisValue)
{
	if(!bEnableInputControl) return;
	
	if(controllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(controllingPawn);
		character->Turn(NewAxisValue);
	}
	AddYawInput(NewAxisValue);
}

void ALocalPlayerController::FKeyPressed()
{
	if(!bEnableInputControl) return;

	
	if(controllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(controllingPawn);
		auto state = character->GetCharacterStateComponent();
		if(!state->IsEquippable()) return;
		
		if(!state->IsEquipped())
		{
			character->EquipWeapon(character->OverlappedWeapon);
		}
		else // equipped
		{
			character->EquipWeapon(character->OverlappedWeapon);
		}
	}
	// We could write some code below here when we tend to implement ridings or sth else
}

void ALocalPlayerController::OnAttack()
{
	if(controllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(controllingPawn);
		auto state = character->GetCharacterStateComponent();

		// combo hit
		if(state->GetCurrentMode() == CharacterMode::NON_EQUIPPED)
		{
			character->AttackNonEquip();
			bEnableInputControl = false;
		}
		// swing
		if(state->GetCurrentMode() == CharacterMode::MELEE)
		{
			character->SwingMelee();
		}
		// shot
		if(state->GetCurrentMode() == CharacterMode::GUN)
		{
			character->Shoot();
		}
	}
}

void ALocalPlayerController::OnAttackStop()
{
	if(controllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(controllingPawn);
		auto state = character->GetCharacterStateComponent();
		if(state->GetCurrentMode() == CharacterMode::NON_EQUIPPED) 
			bEnableInputControl = true;
		if(state->GetCurrentMode() == CharacterMode::GUN) 
			character->StopShooting();
	}
}

void ALocalPlayerController::OnReload()
{
	if(controllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(controllingPawn);
		auto state = character->GetCharacterStateComponent();
		if(state->GetCurrentMode() == CharacterMode::GUN) 
			character->ReloadGun();
	}
}

