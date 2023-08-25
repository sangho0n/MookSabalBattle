// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalPlayerController.h"
#include "PlayerCharacter.h"

void ALocalPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ALocalPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	controllingPawn = InPawn;
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

}

void ALocalPlayerController::ForwardBack(float NewAxisValue)
{
	if(controllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(controllingPawn);
		character->ForwardBack(NewAxisValue);
	}
}

void ALocalPlayerController::LeftRight(float NewAxisValue)
{
	if(controllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(controllingPawn);
		character->LeftRight(NewAxisValue);
	}
}

void ALocalPlayerController::SpacebarPressed()
{
	if(controllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(controllingPawn);
		character->Jump();
	}
}

void ALocalPlayerController::MouseVerticalChange(float NewAxisValue)
{
	if(controllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(controllingPawn);
		character->LookUp(NewAxisValue);
	}
	AddPitchInput(NewAxisValue);
}

void ALocalPlayerController::MouseHorizontalChange(float NewAxisValue)
{
	if(controllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(controllingPawn);
		character->Turn(NewAxisValue);
	}
	AddYawInput(NewAxisValue);
}

void ALocalPlayerController::FKeyPressed()
{
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

