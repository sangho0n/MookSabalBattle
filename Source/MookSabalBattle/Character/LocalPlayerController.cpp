// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalPlayerController.h"

#include "MSBAnimInstance.h"
#include "PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

void ALocalPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	bEnableInputControl = false;
	bIsPossessingPawnInitialized = false;
}

void ALocalPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALocalPlayerController, bEnableInputControl);
}


void ALocalPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	bEnableInputControl = false;
	bIsPossessingPawnInitialized = false;
}

void ALocalPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAxis("MoveForward", this, &ALocalPlayerController::ForwardBack);
	InputComponent->BindAxis("MoveRight", this, &ALocalPlayerController::LeftRight);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ALocalPlayerController::Jump);
	InputComponent->BindAxis("LookUp", this, &ALocalPlayerController::MouseVerticalChange);
	InputComponent->BindAxis("Turn", this, &ALocalPlayerController::MouseHorizontalChange);
	InputComponent->BindAction("Equip", IE_Pressed, this, &ALocalPlayerController::Equip);
	InputComponent->BindAction("Attack", IE_Pressed, this, &ALocalPlayerController::Attack);
	InputComponent->BindAction("Attack", IE_Released, this, &ALocalPlayerController::AttackStop);
	InputComponent->BindAction("Reload", IE_Released, this, &ALocalPlayerController::Reload);
}

void ALocalPlayerController::ForwardBack(float NewAxisValue)
{
	if(!bEnableInputControl) return;
	if(!bIsPossessingPawnInitialized) return;

	auto ControllingPawn = GetPawn();
	if(ControllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(ControllingPawn);
		character->ForwardBack(NewAxisValue);
	}
}

void ALocalPlayerController::LeftRight(float NewAxisValue)
{
	if(!bEnableInputControl) return;
	if(!bIsPossessingPawnInitialized) return;
	
	auto ControllingPawn = GetPawn();
	if(ControllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(ControllingPawn);
		character->LeftRight(NewAxisValue);

	}
}

void ALocalPlayerController::Jump()
{
	if(!bEnableInputControl) return;
	if(!bIsPossessingPawnInitialized) return;
	
	auto ControllingPawn = GetPawn();
	if(ControllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(ControllingPawn);
		character->Jump();

	}
}

void ALocalPlayerController::MouseVerticalChange(float NewAxisValue)
{
	if(!bEnableInputControl) return;
	if(!bIsPossessingPawnInitialized) return;
	
	auto ControllingPawn = GetPawn();
	if(ControllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(ControllingPawn);
		character->LookUp(NewAxisValue);
	}
	AddPitchInput(NewAxisValue);
}

void ALocalPlayerController::MouseHorizontalChange(float NewAxisValue)
{
	if(!bEnableInputControl) return;
	if(!bIsPossessingPawnInitialized) return;
	
	auto ControllingPawn = GetPawn();
	if(ControllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(ControllingPawn);
		character->Turn(NewAxisValue);
	}
	AddYawInput(NewAxisValue);
}

void ALocalPlayerController::Equip()
{
	if(!bEnableInputControl) return;
	if(!bIsPossessingPawnInitialized) return;
	
	auto ControllingPawn = GetPawn();
	if(ControllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(ControllingPawn);
		auto state = character->GetCharacterStateComponent();
		MSB_LOG(Warning, TEXT("is equippable? %s"), state->IsEquippable()?TEXT("true"):TEXT("false"));
		if(!state->IsEquippable()) return;
		MSB_LOG_LOCATION(Warning);

		if(!state->bIsEquipped)
		{
			character->EquipWeapon_Server(character->OverlappedWeapon);
		}
		else // equipped
		{
			character->EquipWeapon_Server(character->OverlappedWeapon);
		}
	}
	// We could write some code below here when we tend to implement ridings or sth else
}

void ALocalPlayerController::Attack()
{	
	if(!bIsPossessingPawnInitialized) return;
	
	auto ControllingPawn = GetPawn();
	if(ControllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(ControllingPawn);
		auto state = character->GetCharacterStateComponent();

		// combo hit
		if(state->CurrentMode == CharacterMode::NON_EQUIPPED)
		{
			bEnableInputControl = false;
			character->AttackNonEquip_Server();
		}
		// swing
		if(state->CurrentMode == CharacterMode::MELEE)
		{
			character->SwingMelee_Server();
		}
		// shot
		if(state->CurrentMode == CharacterMode::GUN)
		{
			character->Shoot_Server();
		}
	}
}

void ALocalPlayerController::AttackStop()
{
	if(!bIsPossessingPawnInitialized) return;
	
	auto ControllingPawn = GetPawn();
	if(ControllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(ControllingPawn);
		auto state = character->GetCharacterStateComponent();
		if(state->CurrentMode == CharacterMode::NON_EQUIPPED) 
			bEnableInputControl = true;
		if(state->CurrentMode == CharacterMode::GUN) 
			character->StopShooting_Server();
	}
}

void ALocalPlayerController::Reload()
{
	if(!bEnableInputControl) return;
	if(!bIsPossessingPawnInitialized) return;
	
	auto ControllingPawn = GetPawn();
	if(ControllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(ControllingPawn);
		auto state = character->GetCharacterStateComponent();
		if(state->CurrentMode == CharacterMode::GUN) 
			character->ReloadGun_Server();
	}
}

void ALocalPlayerController::InitPlayer()
{
	bIsPossessingPawnInitialized = true;
}
