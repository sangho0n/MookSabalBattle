// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "MSBAnimInstance.h"
#include "PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

ALocalPlayerController::ALocalPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMCNormalRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Main/Input/IMC_Normal.IMC_Normal'"));
	if(IMCNormalRef.Succeeded())
	{
		IMC_Normal = IMCNormalRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IADirMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Main/Input/Actions/IA_DirMove.IA_DirMove'"));
	if(IADirMoveRef.Succeeded())
	{
		IA_DirMove = IADirMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IALookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Main/Input/Actions/IA_Look.IA_Look'"));
	if(IADirMoveRef.Succeeded())
	{
		IA_Look = IALookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IAAttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Main/Input/Actions/IA_Attack.IA_Attack'"));
	if(IADirMoveRef.Succeeded())
	{
		IA_Attack = IAAttackRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IAReloadRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Main/Input/Actions/IA_Reload.IA_Reload'"));
	if(IADirMoveRef.Succeeded())
	{
		IA_Reload = IAReloadRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IAJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Main/Input/Actions/IA_Jump.IA_Jump'"));
	if(IADirMoveRef.Succeeded())
	{
		IA_Jump = IAJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IAEquipRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Main/Input/Actions/IA_Equip.IA_Equip'"));
	if(IADirMoveRef.Succeeded())
	{
		IA_Equip = IAEquipRef.Object;
	}
}


void ALocalPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	DisableInput(this);
	bIsPossessingPawnInitialized = false;
}

void ALocalPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALocalPlayerController, bIsPossessingPawnInitialized);
}


void ALocalPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	DisableInput(this);
	bIsPossessingPawnInitialized = false;
}

void ALocalPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(IMC_Normal, 0);

	auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(IA_DirMove, ETriggerEvent::Triggered, this, &ALocalPlayerController::Move);
	EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ALocalPlayerController::Look);
	EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ALocalPlayerController::Jump);
	EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Triggered, this, &ALocalPlayerController::Attack);
	EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Canceled, this, &ALocalPlayerController::AttackStop);
	EnhancedInputComponent->BindAction(IA_Reload, ETriggerEvent::Triggered, this, &ALocalPlayerController::Reload);
	EnhancedInputComponent->BindAction(IA_Equip, ETriggerEvent::Triggered, this, &ALocalPlayerController::Equip);
	
}

void ALocalPlayerController::Move(const FInputActionValue& Value)
{
	if(!InputEnabled()) return;
	if(!bIsPossessingPawnInitialized) return;
	
	const FVector2D MoveValue = Value.Get<FVector2D>();
	const FRotator MovementRotation(0, GetControlRotation().Yaw, 0);

	auto ControllingPawn = GetPawn();
	if(ControllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(ControllingPawn);
		
		character->DirectionalMove(MoveValue.X, MoveValue.Y);
	}
}

void ALocalPlayerController::Look(const FInputActionValue& Value)
{
	if(!InputEnabled()) return;
	if(!bIsPossessingPawnInitialized) return;
	
	const FVector2D LookValue = Value.Get<FVector2D>();
	
	AddYawInput(LookValue.X);
	AddPitchInput(LookValue.Y);
	auto ControllingPawn = GetPawn();
	if(ControllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(ControllingPawn);
		
		character->LookUp(LookValue.Y);
		character->Turn(LookValue.X);
	}
}


void ALocalPlayerController::Jump()
{
	if(!InputEnabled()) return;
	if(!bIsPossessingPawnInitialized) return;
	
	auto ControllingPawn = GetPawn();
	if(ControllingPawn->IsA(APlayerCharacter::StaticClass()))
	{
		auto character = Cast<APlayerCharacter>(ControllingPawn);
		character->Jump();

	}
}

void ALocalPlayerController::Equip()
{
	if(!InputEnabled()) return;
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
			character->EquipWeapon_Server();
		}
		else // equipped
		{
			character->EquipWeapon_Server();
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
			EnableInput(this);
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
			EnableInput(this);
		else if(state->CurrentMode == CharacterMode::GUN) 
			character->StopShooting_Server();
	}
}

void ALocalPlayerController::Reload()
{
	if(!InputEnabled()) return;
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
	EnableInput(this);
}

void ALocalPlayerController::DisableInputEachCient_Implementation(APlayerController* Controller)
{
	Super::DisableInput(Controller);
}
