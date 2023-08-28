// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "MSBAnimInstance.h"
#include "MookSabalBattle/Weapon/Axe.h"
#include "MookSabalBattle/Weapon/Gun.h"
#include "MookSabalBattle/Weapon/Melee.h"
#include "MookSabalBattle/Weapon/Pick.h"
#include "MookSabalBattle/Weapon/Sword.h"
#include "MookSabalBattle/Weapon/Weapon.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// set cam and spring arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>("CAMERA");
	CharacterState = CreateDefaultSubobject<UCharacterStateComponent>(TEXT("STATE"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	bInterpingCamPos = false;
	CurrentCamPos = FVector::ZeroVector;

	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -88.0f),
		FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));

	// set sk mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_MAN_03(
		TEXT("/Game/Man/Mesh/Full/SK_Man_Full_04.SK_Man_Full_04"));
	if(SK_MAN_03.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_MAN_03.Object);
	}

	// set animation
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ABP_HUMANOID(
		TEXT("/Game/Main/Animation/Humanoid/ABP_Humanoid.ABP_Humanoid_C"));
	if(ABP_HUMANOID.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(ABP_HUMANOID.Class);
	}
	
	// jump config
	auto movement = GetCharacterMovement();\
	movement->AirControl = 0.5f;
	movement->JumpZVelocity = 350.0f;

	CharacterState->SetCurrentMode(CharacterMode::MELEE);
	ChangeCharacterMode(CharacterState->GetCurrentMode());
	CharacterState->SetIsEquipped(false);
	CurrentWeapon = nullptr; // Fist mode

	// UI
	ConstructorHelpers::FClassFinder<UInGameUI> INGAMEUI(TEXT("/Game/Main/UI/WB_InGame.WB_InGame_C"));
	if(INGAMEUI.Succeeded())
	{
		this->InGameUIClass = INGAMEUI.Class;
	}
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if(IsValid(InGameUIClass))
	{
		InGameUI = Cast<UInGameUI>(CreateWidget(GetWorld(), InGameUIClass));
		InGameUI->BindCharacterStat();

		InGameUI->AddToViewport();
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// cam pos change
	if(bInterpingCamPos)
	{
		accTime += DeltaTime;
		CurrentCamPos = FMath::Lerp(CurrentCamPos, DesiredCamPos, accTime);
		MSB_LOG(Warning, TEXT("dfdf %s"), *CurrentCamPos.ToString());
		Camera->SetRelativeLocation(CurrentCamPos);
		if(FVector::Dist(DesiredCamPos, CurrentCamPos) < KINDA_SMALL_NUMBER) {bInterpingCamPos = false; accTime = 0.0f;}
	}
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

void APlayerCharacter::Jump()
{
	Super::Jump();
	auto MSBAnimInstacne = Cast<UMSBAnimInstance>(GetMesh()->GetAnimInstance());
	MSBAnimInstacne->SetIntended(true);
}


void APlayerCharacter::LookUp(float NewAxisValue)
{
	// look up animation
}

void APlayerCharacter::Turn(float NewAxisValue)
{

}

void APlayerCharacter::ChangeCharacterMode(CharacterMode NewMode)
{
	CharacterState->SetCurrentMode(NewMode);
	auto CurrentMode = CharacterState->GetCurrentMode();
	if(CurrentMode == CharacterMode::NON_EQUIPPED || CurrentMode == CharacterMode::MELEE)
	{
		// 3rd view mouse rotation
		SpringArm->TargetArmLength = 400.0f;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		// Don't rotate when the controller rotates. Let that just affect the camera.
		this->bUseControllerRotationPitch = false;
		this->bUseControllerRotationRoll = false;
		this->bUseControllerRotationYaw = false;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		SpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller
		bInterpingCamPos = true;
		DesiredCamPos = FVector::ZeroVector;
		accTime = 0.0f;
		InGameUI->SetAimInvisible();
		return;
	}
	if(CurrentMode == CharacterMode::GUN)
	{
		// 3rd view mouse rotation
		SpringArm->TargetArmLength = 400.0f;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		// Don't rotate when the controller rotates. Let that just affect the camera.
		this->bUseControllerRotationPitch = false;
		this->bUseControllerRotationRoll = false;
		this->bUseControllerRotationYaw = true;
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		SpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller
		bInterpingCamPos = true;
		DesiredCamPos = CamPosWhenGunMode;
		accTime = 0.0f;
		InGameUI->SetAimVisible();
		return;
	}
}

CharacterMode APlayerCharacter::GetCurrentMode()
{
	return CharacterState->GetCurrentMode();
}

bool APlayerCharacter::EquipWeapon(AWeapon* NewWeapon)
{
	if(nullptr != CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}
	
	CurrentWeapon = NewWeapon;
	CharacterState->SetIsEquipped(true);
	if(CurrentWeapon->IsA(AGun::StaticClass()))
	{
		ChangeCharacterMode(CharacterMode::GUN);
		FName socket("hand_rifle_rSocket");
		if(GetMesh()->DoesSocketExist(socket))
		{
			auto weaponMesh = CurrentWeapon->GetWeaponMesh();
			weaponMesh->SetSimulatePhysics(false);
			weaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			weaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, socket);

			auto weaponTransform = weaponMesh->GetRelativeTransform();
			FQuat rot3_yaw(FVector(0, 0, 1), FMath::DegreesToRadians(3.0f));
			FQuat rot6_5_pitch(FVector(0, 1, 0), FMath::DegreesToRadians(6.5f));
			weaponTransform.ConcatenateRotation(rot3_yaw);
			weaponTransform.ConcatenateRotation(rot6_5_pitch);
			weaponMesh->SetRelativeTransform(weaponTransform);
		}
	}
	else if (CurrentWeapon->IsA(AMelee::StaticClass()))
	{
		ChangeCharacterMode(CharacterMode::MELEE);
		FName socket("hand_sword_rSocket");
		if(GetMesh()->DoesSocketExist(socket))
		{
			auto weaponMesh = CurrentWeapon->GetWeaponMesh();
			weaponMesh->SetSimulatePhysics(false);
			weaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			if(CurrentWeapon->IsA(AAxe::StaticClass()) || CurrentWeapon->IsA(APick::StaticClass()))
			{
				weaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, socket);
				
				auto weaponTransform = weaponMesh->GetRelativeTransform();
				FQuat rot180(FVector(0, 0, 1), FMath::DegreesToRadians(180.0f));
				weaponTransform.ConcatenateRotation(rot180);
				weaponMesh->SetRelativeTransform(weaponTransform);
			}
			else
			{
				weaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, socket);
			}
		}
	}
	return true;
}

void APlayerCharacter::OnWeaponStartOverlap(AWeapon* OverlappedWeapon_)
{
	InGameUI->SetEquipVisible();
	CharacterState->SetIsEquippable(true);
	this->OverlappedWeapon = OverlappedWeapon_;
}

void APlayerCharacter::OnWeaponEndOverlap()
{
	InGameUI->SetEquipInvisible();
	CharacterState->SetIsEquippable(false);
}

UCharacterStateComponent* APlayerCharacter::GetCharacterStateComponent()
{
	return this->CharacterState;
}

