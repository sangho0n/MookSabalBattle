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
#include "LocalPlayerController.h"
#include "Blueprint/UserWidgetBlueprint.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "MookSabalBattle/MookSabalBattleGameModeBase.h"
#include "MookSabalBattle/MSBGameInstance.h"
#include "MookSabalBattle/MSBGameStateBase.h"
#include "MookSabalBattle/SessionManip/NullSessionSubsystem.h"

int APlayerCharacter::InitFinishedPlayer = 0;
FCriticalSection APlayerCharacter::CS_InitFinishedPlayer = FCriticalSection();

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

	// Collision
	AttackCapsuleColliderRadius = 34.0f;
	AttackCapsuleColliderHalfHeight = 30.0f;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> BLEEDING(TEXT("/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Blood/P_Blood_Splat_Cone.P_Blood_Splat_Cone"));
	if(BLEEDING.Succeeded())
	{
		BleedingParticle = BLEEDING.Object;
	}
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	GetMesh()->SetCollisionProfileName(TEXT("Humanoid"));
	OverlappedAlly = {};
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	auto Collider = GetCapsuleComponent();

	Collider->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnCharacterBeginOverlapWithCharacter);
	Collider->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnCharacterEndOverlapWithCharacter);
	OnGetDamage.AddDynamic(this, &APlayerCharacter::OnHit); // blueprint
	Cast<UMSBAnimInstance>(GetMesh()->GetAnimInstance())->OnAttackEnd.AddDynamic(this, &APlayerCharacter::StopAttacking);
	
	PunchDamage = 7.0f;
	KickDamage = 16.0f;
}


// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if(IsLocallyControlled()) Cast<UMSBGameInstance>(GetGameInstance())->StartLoading();
	GetMesh()->SetVisibility(false);

	// wait for some seconds for replications
	FTimerHandle TimerHandle;
	float DelayInSeconds = 2.0f;
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &APlayerCharacter::AfterReplication);
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, DelayInSeconds, false);
}

void APlayerCharacter::AfterReplication()
{
	// 맵에 있는 무기의 수만큼 PlayerCharacter 클래스의 BeginPlay가 추가적으로 호출되는 문제 확인
	// 컨트롤러의 null 여부로 임시 해결
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
		FString::Printf(TEXT("is null? : %s"), nullptr == this->GetController()?TEXT("null"):TEXT("not null")));
	if(nullptr == this->GetController()) return;
	
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("has Auth? : %s"), HasAuthority()?TEXT("true"):TEXT("false")));

	if(HasAuthority())
	{
		auto GameMode = Cast<AMookSabalBattleGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		OnNewPlayerReplicationFinished.BindUObject(GameMode, &AMookSabalBattleGameModeBase::IncreaseRepFinishedPlayerCount);
		OnNewPlayerReplicationFinished.Execute();
	}
}

void APlayerCharacter::Respawn()
{
	GetMesh()->SetVisibility(true);
	CharacterState->Reset();
	ChangeCharacterMode(CharacterMode::NON_EQUIPPED);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	GetMesh()->SetCollisionProfileName(TEXT("Humanoid"));
	
	if(IsLocallyControlled())
	{
		auto LocalController = Cast<ALocalPlayerController>(GetController());
		LocalController->InitPlayer();
		LocalController->EnableInput(LocalController);
	}

	// respawn at its own PlayerStart
	if(HasAuthority())
	{
		auto GameMode = Cast<AMookSabalBattleGameModeBase>(GetWorld()->GetAuthGameMode());
		APlayerStart* PlayerStart = Cast<APlayerStart>(GameMode->ChoosePlayerStart(GetController()));
		SetActorLocationAndRotation(
			PlayerStart->GetActorLocation(),
			PlayerStart->GetActorRotation()
		);
	}
}

// NetMulticast RPC (called on both listen server and client)
void APlayerCharacter::InitPlayer_Implementation(bool bIsRedTeam)
{
	GetMesh()->SetVisibility(true);
	CharacterState = Cast<ACharacterState>(GetPlayerState());
	auto GameInstance = Cast<UMSBGameInstance>(GetGameInstance());
	check(CharacterState);
	check(GameInstance);
	
	if(HasAuthority())
	{
		CharacterState->OnHPIsZero.AddDynamic(this, &APlayerCharacter::Die_Server);
		CharacterState->SetTeam(bIsRedTeam);
		CharacterState->SetDead(false);
	}

	InitWidgets();
	ChangeCharacterMode(CharacterMode::NON_EQUIPPED);
	
	if(IsLocallyControlled())
	{
		Cast<ALocalPlayerController>(GetController())->InitPlayer();
		Cast<UMSBGameInstance>(GetGameInstance())->EndLoading();
	}

	CS_InitFinishedPlayer.Lock();
	InitFinishedPlayer++;
	if(InitFinishedPlayer == GameInstance->GetSubsystem<UNullSessionSubsystem>()->MaxPlayer)
	{
		// wait for some seconds for bIsRedTeam replication
		FTimerHandle TimerHandle;
		float DelayInSeconds = 0.4f;
		FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &APlayerCharacter::SetPlayerOutline);
		GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, DelayInSeconds, false);
	}
	CS_InitFinishedPlayer.Unlock();
}

void APlayerCharacter::InitWidgets_Implementation()
{
	// InGame UI
	if(IsLocallyControlled())
	{
		if(IsValid(InGameUIClass))
		{
			InGameUI = Cast<UInGameUI>(CreateWidget(GetWorld(), InGameUIClass));
			InGameUI->AddToViewport();
			MSB_LOG_LOCATION(Warning);
			InGameUI->BindCharacterStat(CharacterState);
			Cast<AMSBGameStateBase>(GetWorld()->GetGameState())->PlayGame();
		}

		if(IsValid(EndGameUIClass))
		{
			EndGameUI = CreateWidget(GetWorld(), EndGameUIClass);
			EndGameUI->AddToViewport();
			EndGameUI->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// HealthBar(by blueprint)
}


void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, CharacterState);
}

// act like a static method for setting outline of player characters
// only called once for each client(or listen server)
void APlayerCharacter::SetPlayerOutline()
{
	TArray<TObjectPtr<APlayerState>> Players = GetWorld()->GetGameState()->PlayerArray;
	APlayerCharacter* LocalPlayer = nullptr;

	for(auto Player : Players)
	{
		if(Player->GetPawn()->IsLocallyControlled())
		{
			LocalPlayer = Cast<APlayerCharacter>(Player->GetPawn());
		}
	}

	for(int i = 0; i < Players.Num(); i++)
	{
		APlayerCharacter* Character = Cast<APlayerCharacter>(Players[i]->GetPawn());
		if(Character->IsLocallyControlled()) continue;
		
		Character->GetMesh()->SetRenderCustomDepth(true);
		if(LocalPlayer->IsSameTeam(Character))
		{
			Character->GetMesh()->SetCustomDepthStencilValue(OUT_LINE::Ally);
		}
		else
		{
			Character->GetMesh()->SetCustomDepthStencilValue(OUT_LINE::Enemy);
		}
	}
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

	if(!OverlappedAlly.IsEmpty())
	{
		for(auto OtherActor : OverlappedAlly)
		{
			auto CurrentPos = GetActorLocation();
			auto OtherPos = OtherActor->GetActorLocation();
			auto ToOther = OtherPos - CurrentPos; ToOther.Normalize();

			GetMovementComponent()->Velocity += -ToOther * 10.0f;
		}
	}
}

void APlayerCharacter::ForwardBack(float NewAxisValue)
{
	if(NewAxisValue != 0.0)
	{
		const auto Rotation = Controller->GetControlRotation();
		const FRotator XYRotation(0, Rotation.Yaw, 0);

		const FVector Forward = FRotationMatrix(XYRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Forward, NewAxisValue);
		//GetCharacterMovement()->AddInputVector(Forward*NewAxisValue, false);
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

void APlayerCharacter::EquipWeapon_Server_Implementation(AWeapon* NewWeapon)
{
	// if Already possessed by other character or not available
	MSB_LOG(Warning,TEXT("b is possesse %s, is valid %s, is equippable %s")
		,NewWeapon->bIsPossessed?TEXT("true"):TEXT("false")
		,IsValid(NewWeapon)?TEXT("true"):TEXT("false"),
		CharacterState->IsEquippable()?TEXT("true"):TEXT("false"));
	if(NewWeapon->bIsPossessed || !IsValid(NewWeapon) || !CharacterState->IsEquippable()) return;

	EquipWeapon_Multicast();
}

void APlayerCharacter::EquipWeapon_Multicast_Implementation()
{
	MSB_LOG(Warning, TEXT("multicasted"));
	if(nullptr != CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}
	CurrentWeapon = OverlappedWeapon;
	auto weaponMesh = CurrentWeapon->ReadyToEquip(this);

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
	CurrentWeapon->SetOwner(this);
}

void APlayerCharacter::OnWeaponStartOverlap_Server_Implementation(AWeapon* OverlappedWeapon_)
{
	OnWeaponStartOverlap_Multicast(OverlappedWeapon_);
}

void APlayerCharacter::OnWeaponStartOverlap_Multicast_Implementation(AWeapon* OverlappedWeapon_)
{
	CharacterState->SetEquippable(true);
	this->OverlappedWeapon = OverlappedWeapon_;
	if(IsLocallyControlled())
		InGameUI->SetEquipVisible();
}

void APlayerCharacter::OnWeaponEndOverlap_Server_Implementation()
{
	OnWeaponEndOverlap_Multicast();
}

void APlayerCharacter::OnWeaponEndOverlap_Multicast_Implementation()
{
	CharacterState->SetEquippable(false);
	if(IsLocallyControlled())
		InGameUI->SetEquipInvisible();
}


ACharacterState* APlayerCharacter::GetCharacterStateComponent()
{
	return this->CharacterState;
}

# pragma region attack
void APlayerCharacter::AttackNonEquip_Server_Implementation()
{
	AttackNonEquip_Multicast();
}

void APlayerCharacter::AttackNonEquip_Multicast_Implementation()
{
	auto animInstance = Cast<UMSBAnimInstance>(GetMesh()->GetAnimInstance());
	if(CharacterState->IsAttacking())
	{
		animInstance->SetNextComboInputOn(true);
	}
	else
	{
		CharacterState->SetAttacking(true);
		animInstance->PlayComboAnim();
	}
}

void APlayerCharacter::Shoot_Server_Implementation()
{
	if(CharacterState->IsAttacking() || CharacterState->IsReloading()) return;
	Shoot_Multicast();
}

void APlayerCharacter::Shoot_Multicast_Implementation()
{
	auto Gun = Cast<AGun>(CurrentWeapon);
	if(!Gun->CanFire(this)) return;
	
	if(IsLocallyControlled() && !bInterpingCamPos)
	{
		bInterpingCamPos = true;
		DesiredCamPos = this->CamPosWhenFireGun;
	}
	
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed / 2.0f;
	CharacterState->SetAttacking(true);
}

void APlayerCharacter::StopShooting_Server_Implementation()
{
	StopShooting_Multicast();
}

void APlayerCharacter::StopShooting_Multicast_Implementation()
{
	if(IsLocallyControlled())
	{
		bInterpingCamPos = true;
		DesiredCamPos = this->CamPosWhenGunMode;
	}

	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	CharacterState->SetAttacking(false);
}

void APlayerCharacter::SwingMelee_Server_Implementation()
{
	if(CharacterState->IsAttacking()) return;
	SwingMelee_Multicast();
}

void APlayerCharacter::SwingMelee_Multicast_Implementation()
{
	auto animInstance = Cast<UMSBAnimInstance>(GetMesh()->GetAnimInstance());
	animInstance->PlayRandomMeleeSwing();

	CharacterState->SetAttacking(true);
}

void APlayerCharacter::ReloadGun_Server_Implementation()
{
	auto Gun = Cast<AGun>(CurrentWeapon);
	if(CharacterState->IsReloading()
		|| CharacterState->IsAttacking()
		|| Gun->Bullets >= 45) return;

	ReloadGun_Multicast();
}

void APlayerCharacter::ReloadGun_Multicast_Implementation()
{
	CharacterState->SetReload(true);
}

void APlayerCharacter::Hit(int32 CurrCombo)
{
	if(!IsLocallyControlled()) return;
	
	auto currentMode = CharacterState->CurrentMode;
	TArray<FPointDamageEvent> DamageEvents = {}; 
	if(currentMode == CharacterMode::NON_EQUIPPED)
	{
		if (CurrCombo <= 2) DamageEvents.Append(Punch());
		else  DamageEvents.Append(Kick());
	}
	else if(currentMode == CharacterMode::MELEE)
	{
		auto Melee = Cast<AMelee>(CurrentWeapon);
		DamageEvents.Append(Melee->Hit(this));
	} 
	else // gun
	{
		auto Gun = Cast<AGun>(CurrentWeapon);
		auto DamageEvent = Gun->Hit(this);
		DamageEvents.Add(DamageEvent);
		
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
	
	ApplyDamageEventsOnServer(this, DamageEvents);
}

TArray<FPointDamageEvent> APlayerCharacter::Punch()
{
	TArray<FHitResult> HitResults;
	TArray<FPointDamageEvent> DamageEvents;
	auto Param_IgnoreSelf = FCollisionQueryParams();
	Param_IgnoreSelf.AddIgnoredActor(this);

	auto bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * (AttackCapsuleColliderHalfHeight + AttackCapsuleColliderRadius) * 2,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		ECollisionChannel::ECC_GameTraceChannel4,
		FCollisionShape::MakeCapsule(AttackCapsuleColliderRadius, AttackCapsuleColliderHalfHeight),
		Param_IgnoreSelf);

	if(bResult)
	{
		TArray<APlayerCharacter*> AlreadyHitActors;
		for(auto res: HitResults)
		{
			if(!res.GetActor()->IsA(APlayerCharacter::StaticClass())) continue;
			
			auto Character = Cast<APlayerCharacter>(res.GetActor());
			if(AlreadyHitActors.Contains(Character) || Character->IsSameTeam(this)) continue;
			
			AlreadyHitActors.Push(Character);
			auto ToThis = this->GetActorLocation() - res.Location; ToThis.Normalize();
			FPointDamageEvent PointDamageEvent;
			PointDamageEvent.Damage = PunchDamage;
			PointDamageEvent.HitInfo = res;
			PointDamageEvent.ShotDirection = ToThis;

			//Character->TakeDamage(PunchDamage, PointDamageEvent, this->GetInstigatorController(), this);
			DamageEvents.Add(PointDamageEvent);
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

	return DamageEvents;
}

TArray<FPointDamageEvent> APlayerCharacter::Kick()
{
	TArray<FHitResult> HitResults;
	TArray<FPointDamageEvent> DamageEvents;
	auto Param_IgnoreSelf = FCollisionQueryParams::DefaultQueryParam;
	Param_IgnoreSelf.AddIgnoredActor(this);

	auto bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * (AttackCapsuleColliderHalfHeight + AttackCapsuleColliderRadius) * 2,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		ECollisionChannel::ECC_GameTraceChannel4,
		FCollisionShape::MakeCapsule(AttackCapsuleColliderRadius, AttackCapsuleColliderHalfHeight),
		Param_IgnoreSelf);
		
	if(bResult)
	{
		TArray<APlayerCharacter*> AlreadyHitActors;
		for(auto res: HitResults)
		{
			if(!res.GetActor()->IsA(APlayerCharacter::StaticClass())) continue;

			auto Character = Cast<APlayerCharacter>(res.GetActor());
			if(AlreadyHitActors.Contains(Character)|| Character->IsSameTeam(this)) continue;
			
			AlreadyHitActors.Push(Character);
			auto ToThis = this->GetActorLocation() - res.Location; ToThis.Normalize();
			FPointDamageEvent PointDamageEvent;
			PointDamageEvent.Damage = KickDamage;
			PointDamageEvent.HitInfo = res;
			PointDamageEvent.ShotDirection = ToThis;

			//Character->TakeDamage(KickDamage, PointDamageEvent, this->GetInstigatorController(), this);
			DamageEvents.Add(PointDamageEvent);
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
	return DamageEvents;
}

// Server RPC
void APlayerCharacter::ApplyDamageEventsOnServer_Implementation(APlayerCharacter* const &Causer, const TArray<FPointDamageEvent> &HitResults)
{
	for(auto Result : HitResults)
	{
		if(Result.Damage < KINDA_SMALL_NUMBER) continue;

		auto DamagedCharacter = Cast<APlayerCharacter>(Result.HitInfo.GetActor());
		DamagedCharacter->TakeDamage(Result.Damage, Result, GetInstigatorController(), Causer);
		DamagedCharacter->MulticastHitEffect(Result, Causer);
		MSB_LOG(Warning, TEXT("%s took %f damage"), *DamagedCharacter->GetName(), Result.Damage);
	}
}

void APlayerCharacter::MulticastHitEffect_Implementation(FPointDamageEvent HitResult, APlayerCharacter* Causer)
{
	auto PointDamageEvent = (FPointDamageEvent*)&HitResult;
	OnGetDamage.Broadcast(PointDamageEvent->HitInfo.BoneName, PointDamageEvent->ShotDirection);
	ShowBleeding(PointDamageEvent->HitInfo.Location, PointDamageEvent->ShotDirection, PointDamageEvent->HitInfo.Normal);

	if(IsLocallyControlled())
	{
		InGameUI->ShowDamageIndicator(Causer);
	}
}



void APlayerCharacter::StopAttacking()
{
	CharacterState->SetAttacking(false);
}

# pragma endregion

/**
 * @brief method for collision handling between players
 * when capsule collider(set as Pawn profile name) begin overlap,
 * they pushes each other
 *
 * If overlapped actor is an ally, it continuously applies a gentle force to push them apart until they separate.
 * If it's an enemy, a strong impulse is applied the moment they overlap to make them immediately move away from each other.
 */
void APlayerCharacter::OnCharacterBeginOverlapWithCharacter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor->IsA(APlayerCharacter::StaticClass())) return;

	auto Character = Cast<APlayerCharacter>(OtherActor);
	if(!Character->IsSameTeam(this))
	{
		auto CurrentPos = GetActorLocation();
		auto OtherPos = OtherActor->GetActorLocation();
		auto ToOther = OtherPos - CurrentPos; ToOther.Normalize();

		GetMovementComponent()->Velocity = -ToOther * 500.0f;
	}
	else
	{
		OverlappedAlly.Add(Character);
	}
}

void APlayerCharacter::OnCharacterEndOverlapWithCharacter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!OtherActor->IsA(APlayerCharacter::StaticClass())) return;

	auto Character = Cast<APlayerCharacter>(OtherActor);
	if(Character->IsSameTeam(this))
	{
		OverlappedAlly.Remove(Character);
	}
}

// should be called on server
float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CharacterState->ApplyDamage(DamageAmount, DamageCauser);
	return DamageAmount;
}

// Server RPC
void APlayerCharacter::Die_Server_Implementation()
{
	auto GameState = Cast<AMSBGameStateBase>(GetWorld()->GetGameState());
	GEngine->AddOnScreenDebugMessage(7, 5.0f, FColor::Red, FString::Printf(TEXT("죽음 서버 rpc")));
	
	if(nullptr != CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}
	GameState->AdjustScore(this);
	CharacterState->DeathCount++;
	
	Die_Multicast();
}
void APlayerCharacter::Die_Multicast_Implementation()
{
	GEngine->AddOnScreenDebugMessage(5, 5.0f, FColor::Red, FString::Printf(TEXT("죽음 멀티캐스트 rpc")));
	
	Cast<UMSBAnimInstance>(GetMesh()->GetAnimInstance())->PlayRandomDeadAnim();
	CharacterState->SetDead(true);
	
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	if(nullptr != CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}
	//OnGetDamage.Clear();

	if(IsLocallyControlled())
	{
		GetLocalViewingPlayerController()->DisableInput(GetLocalViewingPlayerController());
	}

	FTimerHandle TimerHandle;
	float DelayInSeconds = 5.0f;
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &APlayerCharacter::Respawn);
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, DelayInSeconds, false);
	// TODO : blink mesh until to be respawned
}

/**
 * @brief method for show bleeding particles
 * the particle must spawned on the Location.
 * the direction should be a replicated vector of From via Normal
 * 
 * @param Location The location that particle spawned
 * @param From A vector from attacking point to Location
 * @param Normal The normal of that mesh at location
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
		Location,
		FRotationMatrix::MakeFromZ(From + 2 * Normal * FVector::DotProduct(-From, Normal)).Rotator(),
		FVector(1),
		true
	);
	MSB_LOG(Warning,TEXT("bleeding"));
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
	CharacterState->SetReload(false);
}

bool APlayerCharacter::IsSameTeam(APlayerCharacter* OtherPlayer)
{
	auto ThisTeam = CharacterState->IsRedTeam();
	auto OtherTeam = OtherPlayer->GetCharacterStateComponent()->IsRedTeam();
	
	if(!(ThisTeam ^ OtherTeam)) return true;
	return false;
}

