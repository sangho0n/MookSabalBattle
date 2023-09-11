// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "MSBAnimInstance.h"
#include "MookSabalBattle/Weapon/Axe.h"
#include "MookSabalBattle/Weapon/Gun.h"
#include "MookSabalBattle/Weapon/Melee.h"
#include "MookSabalBattle/Weapon/Pick.h"
#include "MookSabalBattle/Weapon/Sword.h"
#include "MookSabalBattle/Weapon/Weapon.h"

#include "DrawDebugHelpers.h"

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
	
	// Collision
	SetCharacterAsAlly();
	
	// movement
	auto movement = GetCharacterMovement();\
	movement->AirControl = 0.5f;
	movement->JumpZVelocity = 350.0f;
	movement->MaxWalkSpeed = MaxWalkSpeed;

	CharacterState->SetCurrentMode(CharacterMode::NON_EQUIPPED);
	CharacterState->SetIsEquipped(false);
	CurrentWeapon = nullptr; // Fist mode

	// UI
	ConstructorHelpers::FClassFinder<UInGameUI> INGAMEUI(TEXT("/Game/Main/UI/WB_InGame.WB_InGame_C"));
	if(INGAMEUI.Succeeded())
	{
		this->InGameUIClass = INGAMEUI.Class;
	}

	// Attack Collision
	AttackCapsuleColliderRadius = 34.0f;
	AttackCapsuleColliderHalfHeight = 30.0f;
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	auto Collider = GetCapsuleComponent();
	Collider->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnCharacterBeginOverlapWithCharacter);
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
	ChangeCharacterMode(CharacterState->GetCurrentMode());
	
	// below code will be refactored after implement server
	if(!GetController()->IsLocalController())
	{
		SetCharacterAsEnemy();
	}
}

void APlayerCharacter::SetCharacterAsAlly()
{
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Ally"));
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
	CharacterState->SetFriendly(true);
}

void APlayerCharacter::SetCharacterAsEnemy()
{
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	GetMesh()->SetCollisionProfileName(TEXT("Enemy"));
	CharacterState->SetFriendly(false);
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
	auto yaw = GetControlRotation().Yaw;
	auto animinstance = Cast<UMSBAnimInstance>(GetMesh()->GetAnimInstance());
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
		this->bUseControllerRotationYaw = false;
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
	auto weaponMesh = CurrentWeapon->ReadyToEquip();
	CharacterState->SetIsEquipped(true);
	if(CurrentWeapon->IsA(AGun::StaticClass()))
	{
		ChangeCharacterMode(CharacterMode::GUN);
		FName socket("hand_rifle_rSocket");
		if(GetMesh()->DoesSocketExist(socket))
		{
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
		auto temp = Cast<AMelee>(CurrentWeapon);
		MSB_LOG(Warning, TEXT("curr weapon radius %f, halfheight %f"), temp->AttackCapsuleColliderRadius, AttackCapsuleColliderHalfHeight);
		
		ChangeCharacterMode(CharacterMode::MELEE);
		FName socket("hand_sword_rSocket");
		if(GetMesh()->DoesSocketExist(socket))
		{
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

# pragma region attack
void APlayerCharacter::AttackNonEquip()
{
	auto animInstance = Cast<UMSBAnimInstance>(GetMesh()->GetAnimInstance());
	if(CharacterState->IsAttacking())
	{
		animInstance->SetNextComboInputOn(true);
		return;
	}

	MSB_LOG(Warning, TEXT("Can Next"));
	animInstance->PlayComboAnim();
}

void APlayerCharacter::Shoot()
{
	if(CharacterState->IsAttacking()) return;

	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed / 2.0f * 1.0f;
	
	auto animInstance = Cast<UMSBAnimInstance>(GetMesh()->GetAnimInstance());
	animInstance->PlayShot();
	
}

void APlayerCharacter::StopShooting()
{
	CharacterState->SetIsAttacking(false);

	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void APlayerCharacter::SwingMelee()
{
	if(CharacterState->IsAttacking()) return;
	
	auto animInstance = Cast<UMSBAnimInstance>(GetMesh()->GetAnimInstance());
	animInstance->PlayMeleeSwing();
}

void APlayerCharacter::Hit(int32 CurrCombo)
{
	if(!GetController()->IsLocalPlayerController()) return;
	
	auto currentMode = GetCurrentMode();
	if(currentMode == CharacterMode::NON_EQUIPPED)
	{
		if (CurrCombo <= 2) Punch();
		else Kick();
	}
	else if(currentMode == CharacterMode::MELEE)
	{
		if(CurrentWeapon->IsA(ASword::StaticClass()))
			HitWithSword();
		else if(CurrentWeapon->IsA(AAxe::StaticClass()))
			HitWithAxe();
		else // curr weapon == pick
			HitWithPick();
	}
	else // gun
	{
		HitWithGun();
	}
}

void APlayerCharacter::Punch()
{
	TArray<FHitResult> HitResults;
	auto Param_IgnoreSelf = FCollisionQueryParams::DefaultQueryParam;
	Param_IgnoreSelf.AddIgnoredActor(this);

	auto bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * (AttackCapsuleColliderHalfHeight + AttackCapsuleColliderRadius) * 2,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		ECollisionChannel::ECC_GameTraceChannel4,
		FCollisionShape::MakeCapsule(AttackCapsuleColliderRadius, AttackCapsuleColliderHalfHeight));
		
	if(bResult)
	{
		MSB_LOG(Warning, TEXT("punch res %s"), *HitResults[0].ToString());
	}

#if ENABLE_DRAW_DEBUG
	DrawDebugCapsule(
		GetWorld(),
		GetActorLocation() + GetActorForwardVector() * (AttackCapsuleColliderHalfHeight + AttackCapsuleColliderRadius),
		AttackCapsuleColliderHalfHeight,
		AttackCapsuleColliderRadius,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		bResult ? FColor::Red : FColor::Green,
		false,
		1.0f
	);
#endif
	
}

void APlayerCharacter::Kick()
{
	TArray<FHitResult> HitResults;
	auto Param_IgnoreSelf = FCollisionQueryParams::DefaultQueryParam;
	Param_IgnoreSelf.AddIgnoredActor(this);

	auto bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * (AttackCapsuleColliderHalfHeight + AttackCapsuleColliderRadius) * 2,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		ECollisionChannel::ECC_GameTraceChannel4,
		FCollisionShape::MakeCapsule(AttackCapsuleColliderRadius, AttackCapsuleColliderHalfHeight));
		
	if(bResult)
	{
		MSB_LOG(Warning, TEXT("punch res %s"), *HitResults[0].ToString());
	}

#if ENABLE_DRAW_DEBUG
	DrawDebugCapsule(
		GetWorld(),
		GetActorLocation() + GetActorForwardVector() * (AttackCapsuleColliderHalfHeight + AttackCapsuleColliderRadius),
		AttackCapsuleColliderHalfHeight,
		AttackCapsuleColliderRadius,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		bResult ? FColor::Red : FColor::Green,
		false,
		1.0f
	);
#endif
}

void APlayerCharacter::HitWithSword()
{
	TArray<FHitResult> HitResults;
	auto Param_IgnoreSelf = FCollisionQueryParams::DefaultQueryParam;
	Param_IgnoreSelf.AddIgnoredActor(this);
	auto Sword = Cast<ASword>(CurrentWeapon);

	auto bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * (Sword->AttackCapsuleColliderHalfHeight + Sword->AttackCapsuleColliderRadius) * 2,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		ECollisionChannel::ECC_GameTraceChannel4,
		FCollisionShape::MakeCapsule(Sword->AttackCapsuleColliderRadius, Sword->AttackCapsuleColliderHalfHeight));
		
	if(bResult)
	{
		MSB_LOG(Warning, TEXT("punch res %s"), *HitResults[0].ToString());
	}

#if ENABLE_DRAW_DEBUG
	DrawDebugCapsule(
		GetWorld(),
		GetActorLocation() + GetActorForwardVector() * (Sword->AttackCapsuleColliderHalfHeight + Sword->AttackCapsuleColliderRadius),
		Sword->AttackCapsuleColliderHalfHeight,
		Sword->AttackCapsuleColliderRadius,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		bResult ? FColor::Red : FColor::Green,
		false,
		1.0f
	);
#endif
	
}

void APlayerCharacter::HitWithAxe()
{
	TArray<FHitResult> HitResults;
	auto Param_IgnoreSelf = FCollisionQueryParams::DefaultQueryParam;
	Param_IgnoreSelf.AddIgnoredActor(this);
	auto Axe = Cast<AAxe>(CurrentWeapon);

	auto bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * (Axe->AttackCapsuleColliderHalfHeight + Axe->AttackCapsuleColliderRadius) * 2,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		ECollisionChannel::ECC_GameTraceChannel4,
		FCollisionShape::MakeCapsule(Axe->AttackCapsuleColliderRadius, Axe->AttackCapsuleColliderHalfHeight));
		
	if(bResult)
	{
		MSB_LOG(Warning, TEXT("punch res %s"), *HitResults[0].ToString());
	}

#if ENABLE_DRAW_DEBUG
	DrawDebugCapsule(
		GetWorld(),
		GetActorLocation() + GetActorForwardVector() * (Axe->AttackCapsuleColliderHalfHeight + Axe->AttackCapsuleColliderRadius),
		Axe->AttackCapsuleColliderHalfHeight,
		Axe->AttackCapsuleColliderRadius,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		bResult ? FColor::Red : FColor::Green,
		false,
		1.0f
	);
#endif
	
	
}

void APlayerCharacter::HitWithPick()
{
	TArray<FHitResult> HitResults;
	auto Param_IgnoreSelf = FCollisionQueryParams::DefaultQueryParam;
	Param_IgnoreSelf.AddIgnoredActor(this);
	auto Pick = Cast<APick>(CurrentWeapon);

	auto bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * (Pick->AttackCapsuleColliderHalfHeight + Pick->AttackCapsuleColliderRadius) * 2,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		ECollisionChannel::ECC_GameTraceChannel4,
		FCollisionShape::MakeCapsule(Pick->AttackCapsuleColliderRadius, Pick->AttackCapsuleColliderHalfHeight));
		
	if(bResult)
	{
		MSB_LOG(Warning, TEXT("punch res %s"), *HitResults[0].ToString());
	}

#if ENABLE_DRAW_DEBUG
	DrawDebugCapsule(
		GetWorld(),
		GetActorLocation() + GetActorForwardVector() * (Pick->AttackCapsuleColliderHalfHeight + Pick->AttackCapsuleColliderRadius),
		Pick->AttackCapsuleColliderHalfHeight,
		Pick->AttackCapsuleColliderRadius,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		bResult ? FColor::Red : FColor::Green,
		false,
		1.0f
	);
#endif
	
}

void APlayerCharacter::HitWithGun()
{
	MSB_LOG(Warning,TEXT("gun"));
	
	FHitResult HitResult;
	auto Param_IgnoreSelf = FCollisionQueryParams::DefaultQueryParam;
	Param_IgnoreSelf.AddIgnoredActor(this);
	auto Gun = Cast<AGun>(CurrentWeapon);
	auto MuzzlePosInWorld = Gun->GetMuzzleLocationInWS();

	
	static int32 ViewportSizeX, ViewportSizeY; 
	GetLocalViewingPlayerController()->GetViewportSize(ViewportSizeX, ViewportSizeY);
	InGameUI->CrosshairScreenPos = FVector2D(ViewportSizeX / 2, ViewportSizeY / 2);

	FVector CamPosInWorld;
	FVector CamDirInWorld;
	GetLocalViewingPlayerController()->DeprojectScreenPositionToWorld(InGameUI->CrosshairScreenPos.X, InGameUI->CrosshairScreenPos.Y,
		CamPosInWorld, CamDirInWorld);

	auto bResult = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		MuzzlePosInWorld,
		CamPosInWorld + CamDirInWorld * Gun->GunAttackLength,
		ECollisionChannel::ECC_GameTraceChannel4,
		Param_IgnoreSelf
		);
		
	if(bResult)
	{
		MSB_LOG(Warning, TEXT("punch res %s"), *HitResult.ToString());
	}

#if ENABLE_DRAW_DEBUG
	DrawDebugLine(
		GetWorld(),
		MuzzlePosInWorld,
		CamPosInWorld + CamDirInWorld * Gun->GunAttackLength,
		bResult ? FColor::Red : FColor::Green,
		false,
		1.0f
	);
#endif
	
}


# pragma endregion

void APlayerCharacter::OnCharacterBeginOverlapWithCharacter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor->IsA(APlayerCharacter::StaticClass())) return;
	
	auto currentPos = GetActorLocation();
	auto otherPos = OtherActor->GetActorLocation();
	auto toOther = otherPos - currentPos; toOther.Normalize();

	GetMovementComponent()->Velocity = -toOther * 500.0f;
}

