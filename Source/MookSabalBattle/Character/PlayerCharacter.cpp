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
#include "Engine/DamageEvents.h"

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
	
	// movement
	auto movement = GetCharacterMovement();
	movement->AirControl = 0.5f;
	movement->JumpZVelocity = 350.0f;
	movement->MaxWalkSpeed = MaxWalkSpeed;

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
	static ConstructorHelpers::FObjectFinder<UParticleSystem> BLEEDING(TEXT("/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Blood/P_Blood_Splat_Cone.P_Blood_Splat_Cone"));
	if(BLEEDING.Succeeded())
	{
		BleedingParticle = BLEEDING.Object;
	}

}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	auto Collider = GetCapsuleComponent();

	Collider->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnCharacterBeginOverlapWithCharacter);
	OnGetDamage.AddDynamic(this, &APlayerCharacter::OnHit);
	Cast<UMSBAnimInstance>(GetMesh()->GetAnimInstance())->OnAttackEnd.AddDynamic(this, &APlayerCharacter::StopAttacking);
	
	PunchDamage = 7.0f;
	KickDamage = 16.0f;
}


// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TimerHandle;
	float DelayInSeconds = 2.0f;
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &APlayerCharacter::InitPlayer);
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, DelayInSeconds, false);
}

void APlayerCharacter::InitPlayer()
{
	CharacterState = Cast<ACharacterState>(GetPlayerState());
	check(nullptr != CharacterState);
	
	if(HasAuthority())
	{
		CharacterState->OnHPIsZero.AddDynamic(this, &APlayerCharacter::Die);
	}
	else
	{
		FString DebugMsg = FString::Printf(TEXT("나는 클라이언트 폰이야 %s"),
			*GetDebugName(this));
		GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Blue, DebugMsg);
	}
	
	if(IsLocallyControlled())
	{
		if(IsValid(InGameUIClass))
		{
			InGameUI = Cast<UInGameUI>(CreateWidget(GetWorld(), InGameUIClass));
			InGameUI->AddToViewport();
			MSB_LOG_LOCATION(Warning);
			InGameUI->BindCharacterStat(CharacterState);
		}
		// 수정할것
		CharacterState->bIsRedTeam = true;
	}
	else
	{
		CharacterState->bIsRedTeam = false;
	}

	ChangeCharacterMode(CharacterMode::NON_EQUIPPED);
	if(CharacterState->bIsRedTeam)
	{
		SetCharacterAsAlly();
	}
	else
	{
		SetCharacterAsEnemy();
	}
}

void APlayerCharacter::SetCharacterAsAlly()
{
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Ally"));
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
}

void APlayerCharacter::SetCharacterAsEnemy()
{
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	GetMesh()->SetCollisionProfileName(TEXT("Enemy"));
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!IsValid(CharacterState)) return;

	// cam pos change
	if(bInterpingCamPos)
	{
		accTime += DeltaTime;
		CurrentCamPos = FMath::Lerp(CurrentCamPos, DesiredCamPos, accTime);
		Camera->SetRelativeLocation(CurrentCamPos);
		if(FVector::Dist(DesiredCamPos, CurrentCamPos) < KINDA_SMALL_NUMBER) {bInterpingCamPos = false; accTime = 0.0f;}
	}
}

void APlayerCharacter::ForwardBack(float NewAxisValue)
{
	if(NewAxisValue != 0.0)
	{
		const auto Rotation = Controller->GetControlRotation();
		const FRotator XYRotation(0, Rotation.Yaw, 0);

		const auto Forward = FRotationMatrix(XYRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Forward, NewAxisValue);
	}
}

void APlayerCharacter::LeftRight(float NewAxisValue)
{
	if(NewAxisValue != 0.0)
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

}

void APlayerCharacter::Turn(float NewAxisValue)
{

}

void APlayerCharacter::ChangeCharacterMode(CharacterMode NewMode)
{
	CharacterState->CurrentMode = NewMode;
	auto CurrentMode = CharacterState->CurrentMode;
	if(CurrentMode == CharacterMode::NON_EQUIPPED || CurrentMode == CharacterMode::MELEE)
	{
		// Don't rotate when the controller rotates. Let that just affect the camera.
		this->bUseControllerRotationPitch = false;
		this->bUseControllerRotationRoll = false;
		this->bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		if(!IsLocallyControlled()) return;
		// 3rd view mouse rotation
		SpringArm->TargetArmLength = 400.0f;
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
		// Don't rotate when the controller rotates. Let that just affect the camera.
		this->bUseControllerRotationPitch = false;
		this->bUseControllerRotationRoll = false;
		this->bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		if(!IsLocallyControlled()) return;
		// 3rd view mouse rotation
		SpringArm->TargetArmLength = 400.0f;
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

bool APlayerCharacter::EquipWeapon(AWeapon* NewWeapon)
{
	if(nullptr != CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}
	CharacterState->ApplyDamage(50.0f);
	CurrentWeapon = NewWeapon;
	auto weaponMesh = CurrentWeapon->ReadyToEquip();

	CharacterState->bIsEquipped = true;
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

		auto Gun = Cast<AGun>(CurrentWeapon);
		Cast<UMSBAnimInstance>(GetMesh()->GetAnimInstance())->OnReloadAnimEnd
		.AddDynamic(Gun, &AGun::FillBullets);
		Cast<UMSBAnimInstance>(GetMesh()->GetAnimInstance())->OnReloadAnimEnd
		.AddDynamic(this, &APlayerCharacter::EndReloading);
	}
	else if (CurrentWeapon->IsA(AMelee::StaticClass()))
	{
		auto CurrentMelee = Cast<AMelee>(CurrentWeapon);
		
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
	CharacterState->bIsEquippable = true;
	this->OverlappedWeapon = OverlappedWeapon_;
}

void APlayerCharacter::OnWeaponEndOverlap()
{
	InGameUI->SetEquipInvisible();
	CharacterState->bIsEquippable = false;
}

ACharacterState* APlayerCharacter::GetCharacterStateComponent()
{
	return this->CharacterState;
}

# pragma region attack
void APlayerCharacter::AttackNonEquip()
{
	auto animInstance = Cast<UMSBAnimInstance>(GetMesh()->GetAnimInstance());
	if(CharacterState->bIsAttacking)
	{
		animInstance->SetNextComboInputOn(true);
		return;
	}

	CharacterState->bIsAttacking = true;
	animInstance->PlayComboAnim();
}

void APlayerCharacter::Shoot()
{
	if(CharacterState->bIsAttacking || CharacterState->bIsReloading) return;
	auto Gun = Cast<AGun>(CurrentWeapon);
	if(!Gun->CanFire(this)) return;

	bInterpingCamPos = true;
	DesiredCamPos = CamPosWhenFireGun;

	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed / 2.0f * 1.0f;
	CharacterState->bIsAttacking = true;	
}

void APlayerCharacter::StopShooting()
{
	CharacterState->bIsAttacking = false;

	bInterpingCamPos = true;
	DesiredCamPos = CamPosWhenGunMode;

	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void APlayerCharacter::ReloadGun()
{
	auto Gun = Cast<AGun>(CurrentWeapon);
	if(CharacterState->bIsReloading
		|| CharacterState->bIsAttacking
		|| Gun->Bullets >= 45) return;
	
	CharacterState->bIsReloading = true;
}


void APlayerCharacter::SwingMelee()
{
	if(CharacterState->bIsAttacking) return;

	auto animInstance = Cast<UMSBAnimInstance>(GetMesh()->GetAnimInstance());
	animInstance->PlayRandomMeleeSwing();
	CharacterState->bIsAttacking = true;
}

void APlayerCharacter::Hit(int32 CurrCombo)
{
	if(!GetController()->IsLocalPlayerController()) return;
	
	auto currentMode = CharacterState->CurrentMode;
	if(currentMode == CharacterMode::NON_EQUIPPED)
	{
		if (CurrCombo <= 2) Punch();
		else Kick();
	}
	else if(currentMode == CharacterMode::MELEE)
	{
		auto Melee = Cast<AMelee>(CurrentWeapon);
		Melee->Hit(this); // return multiple results
	}
	else // gun
	{
		auto Gun = Cast<AGun>(CurrentWeapon);
		Gun->Hit(this); // return single result
		// ApplyRecoil
		UGameplayStatics::PlayWorldCameraShake(
			Camera,
			Gun->CamShakeShoot,
			GetCameraLocation(),
			0.0f,
			10.0f
		);
		AddControllerPitchInput(-0.5f);
	}
}

void APlayerCharacter::Punch()
{
	TArray<FHitResult> HitResults;
	auto Param_IgnoreSelf = FCollisionQueryParams();
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
		TArray<APlayerCharacter*> AlreadyHitActors;
		for(auto res: HitResults)
		{
			if(!res.GetActor()->IsA(APlayerCharacter::StaticClass())) continue;

			auto Character = Cast<APlayerCharacter>(res.GetActor());
			if(AlreadyHitActors.Contains(Character)) continue;
			
			AlreadyHitActors.Push(Character);
			auto ToThis = this->GetActorLocation() - res.Location; ToThis.Normalize();
			FPointDamageEvent PointDamageEvent;
			PointDamageEvent.Damage = PunchDamage;
			PointDamageEvent.HitInfo = res;
			PointDamageEvent.ShotDirection = ToThis;

			Character->TakeDamage(PunchDamage, PointDamageEvent, this->GetInstigatorController(), this);
		}
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
		TArray<APlayerCharacter*> AlreadyHitActors;
		for(auto res: HitResults)
		{
			if(!res.GetActor()->IsA(APlayerCharacter::StaticClass())) continue;

			auto Character = Cast<APlayerCharacter>(res.GetActor());
			if(AlreadyHitActors.Contains(Character)) continue;
			
			AlreadyHitActors.Push(Character);
			auto ToThis = this->GetActorLocation() - res.Location; ToThis.Normalize();
			FPointDamageEvent PointDamageEvent;
			PointDamageEvent.Damage = KickDamage;
			PointDamageEvent.HitInfo = res;
			PointDamageEvent.ShotDirection = ToThis;

			Character->TakeDamage(KickDamage, PointDamageEvent, this->GetInstigatorController(), this);
		}
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

void APlayerCharacter::StopAttacking()
{
	CharacterState->bIsAttacking = false;
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

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if(DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		auto PointDamageEvent = (FPointDamageEvent*)&DamageEvent;
		OnGetDamage.Broadcast(PointDamageEvent->HitInfo.BoneName, PointDamageEvent->ShotDirection);
		ShowBleeding(PointDamageEvent->HitInfo.Location, PointDamageEvent->ShotDirection, PointDamageEvent->HitInfo.Normal);

		if(this->GetController()->IsLocalPlayerController())
		{
			InGameUI->ShowDamageIndicator(DamageCauser);
		}
	}
	
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CharacterState->ApplyDamage(DamageAmount);
	return DamageAmount;
}

void APlayerCharacter::Die()
{
	Cast<UMSBAnimInstance>(GetMesh()->GetAnimInstance())->PlayRandomDeadAnim();
	CharacterState->bIsDead = true;
	
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
	GetMesh()->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	GetMesh()->SetSimulatePhysics(true);
	OnGetDamage.Clear();
}

/*
 * APlayerCharacter::ShowBleeding
 *
 * Location : The location that particle spawned
 * From : A vector from attacking point to `Location
 * Normal : The normal of that mesh at location
 *
 * the particle must spawned on the `Location,
 * and the rotation angle should be same as the angle between `Normal and `From
 */
void APlayerCharacter::ShowBleeding(FVector Location, FVector From, FVector Normal)
{
	FTransform Transform;
	From.Normalize(); Normal.Normalize();
	Transform.SetLocation(Location);
	Transform.SetRotation(FRotationMatrix::MakeFromZ(
		From + 2 * Normal * FVector::DotProduct(-From, Normal)).ToQuat()
	);
	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		BleedingParticle,
		Transform
	);
}

FVector APlayerCharacter::GetCameraLocation()
{
	return Camera->GetComponentLocation();
}

FVector APlayerCharacter::GetCameraDirection()
{
	return Camera->GetComponentRotation().Vector();
}

void APlayerCharacter::EndReloading()
{
	CharacterState->bIsReloading = false;
}

