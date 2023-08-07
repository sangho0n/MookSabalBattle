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
