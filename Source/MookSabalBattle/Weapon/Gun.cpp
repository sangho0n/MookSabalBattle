// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Engine/DamageEvents.h"

AGun::AGun() : Super()
{
	SM_Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gun"));
	RootComponent = SM_Weapon;
	Collider->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_GUN(TEXT("/Game/MiniSurvivalProps/SM_AssaultRifle.SM_AssaultRifle"));
	if(SM_GUN.Succeeded())
	{
		SM_Weapon->SetStaticMesh(SM_GUN.Object);
	}
	SM_Weapon->SetRelativeLocation(OffsetFromLand);
	SM_Weapon->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0));
	Collider->InitBoxExtent(FVector(45.0, 60.0, 28.0));
	Collider->SetRelativeLocation(FVector(17.0f, 0.0f, 11.0f));

	MuzzleSocket = FName("Muzzle");

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_Particle(TEXT("/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Sparks/P_Sparks_C.P_Sparks_C"));
	if(P_Particle.Succeeded())
	{
		FireParticle = P_Particle.Object;
	}

	static ConstructorHelpers::FClassFinder<UCameraShakeBase> CS_SHOOT(TEXT("/Game/Main/Camera/CS_Shoot.CS_Shoot_C"));
	if(CS_SHOOT.Succeeded())
	{
		CamShakeShoot = CS_SHOOT.Class;
	}
}

void AGun::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Bullets = 45;
	GunAttackLength = 7000.0f;
	GunAttackSpreadAngle = 3.0f;
	Damage = 3.0f;
}

FVector AGun::GetMuzzleLocationInWS()
{
	return SM_Weapon->GetSocketLocation(MuzzleSocket);
}

void AGun::FireParticleOnMuzzle()
{
	UGameplayStatics::SpawnEmitterAttached(FireParticle,
		SM_Weapon,
		MuzzleSocket,
		FVector(0, 0, 0),
		FRotator(-90.0f, 0, 0),
		FVector(0.2f)
		);
}

FHitResult AGun::Hit(APlayerCharacter* Causer)
{
	FHitResult HitResult;
	auto Param_IgnoreSelf = FCollisionQueryParams::DefaultQueryParam;
	Param_IgnoreSelf.AddIgnoredActor(this);
	Param_IgnoreSelf.bTraceComplex = true;
	
	if(!CanFire(Causer)) { return HitResult;}
	Bullets--;
	MSB_LOG(Warning, TEXT("current bullets : %d"), Bullets);

	FireParticleOnMuzzle();
	bool bHit = false;
	bool bBlocked = false;
	auto CameraLocation = Causer->GetCameraLocation();
	auto TargetPos = CameraLocation + Causer->GetCameraDirection() * GunAttackLength;
	
	bBlocked = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		CameraLocation,
		TargetPos,
		ECollisionChannel::ECC_GameTraceChannel4,
		Param_IgnoreSelf
		);

	if(bBlocked)
	{
		if(!HitResult.GetActor()->IsA(APlayerCharacter::StaticClass())) {bHit = false;}
		else
		{
			bHit = true;
			auto Character = Cast<APlayerCharacter>(HitResult.GetActor());

			auto ToThis = this->GetActorLocation() - HitResult.Location; ToThis.Normalize();
			FPointDamageEvent PointDamageEvent;
			PointDamageEvent.Damage = this->Damage;
			PointDamageEvent.HitInfo = HitResult;
			PointDamageEvent.ShotDirection = ToThis;

			MSB_LOG(Warning, TEXT("shot %s with gun"), *HitResult.GetActor()->GetName());
			Character->TakeDamage(this->Damage, PointDamageEvent, Causer->GetInstigatorController(), Causer);
		}
	}
#if ENABLE_DRAW_DEBUG
	DrawDebugLine(
		GetWorld(),
		CameraLocation,
		TargetPos,
		bBlocked ? (bHit ? FColor::Red : FColor:: Magenta) : FColor::Green,
		false,
		1.0f
	);
#endif

	return HitResult;
}

bool AGun::CanFire(APlayerCharacter* Causer)
{
	if(Bullets <= 0) {Causer->StopShooting(); return false;}
	return true;
}

void AGun::ReloadGun()
{
	Bullets = 45;
}
