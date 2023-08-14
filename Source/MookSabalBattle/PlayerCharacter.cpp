// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// set cam and spring arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>("CAMERA");

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -88.0f),
		FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	// set sk mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_MAN_03(
		TEXT("/Game/Man/Mesh/Full/SK_Man_Full_04.SK_Man_Full_04"));
	if(SK_MAN_03.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_MAN_03.Object);
	}

	// set animation
	// TODO: create an animation blueprint 1. jump & fall 2. run 3. rotate(movement of lover body)
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ABP_HUMANOID(
		TEXT("/Game/Main/Animation/Humanoid/ABP_Humanoid.ABP_Humanoid_C"));
	MSB_LOG(Warning, TEXT("%d"), ABP_HUMANOID.Succeeded());
	if(ABP_HUMANOID.Succeeded())
	{
		MSB_LOG(Warning, TEXT("dddddd"));
		GetMesh()->SetAnimInstanceClass(ABP_HUMANOID.Class);
	}
	
	// jump config
	auto movement = GetCharacterMovement();\
	movement->AirControl = 0.5f;
	movement->JumpZVelocity = 350.0f;

	this->ChangeCharacterMode(CharacterMode::NON_EQUIPPED);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

void APlayerCharacter::ForwardBack(float NewAxisValue)
{
	if(nullptr != Controller && NewAxisValue != 0.0)
	{
		const auto Rotation = Controller->GetControlRotation();
		const FRotator XYRotation(0, Rotation.Yaw, 0);

		const auto Forward = FRotationMatrix(XYRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Forward, NewAxisValue);
	}
}

void APlayerCharacter::LeftRight(float NewAxisValue)
{
	if(nullptr != Controller && NewAxisValue != 0.0)
	{
		const auto Rotation = Controller->GetControlRotation();
		const FRotator XYRotation(0, Rotation.Yaw, 0);

		const auto Forward = FRotationMatrix(XYRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Forward, NewAxisValue);
	}
}

void APlayerCharacter::LookUp(float NewAxisValue)
{
	// look up animation
}

void APlayerCharacter::Turn(float NewAxisValue)
{
	this->AddControllerYawInput(NewAxisValue);
}

void APlayerCharacter::ChangeCharacterMode(CharacterMode NewMode)
{
	// if(CurrentMode == NewMode && CurrentMode != CharacterMode::NULL)
	// {
	// 	MSB_LOG(Warning, TEXT("Already has same mode!!"));
	// 	return;
	// }
	CurrentMode = NewMode;
	if(CurrentMode == CharacterMode::NON_EQUIPPED)
	{
		// 3rd view mouse rotation
		SpringArm->TargetArmLength = 400.0f;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		// Don't rotate when the controller rotates. Let that just affect the camera.
		this->bUseControllerRotationPitch = false;
		this->bUseControllerRotationRoll = false;
		//this->bUseControllerRotationYaw = false; 블랜딩 에니메이션
		this->bUseControllerRotationYaw = true;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		SpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller
		return;
	}
	if(CurrentMode == CharacterMode::GUN)
	{
		this->bUseControllerRotationYaw = true;
		return;
	}
}

CharacterMode APlayerCharacter::GetCurrentMode()
{
	return CurrentMode;
}
