// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"

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

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGun, Bullets);
}


void AGun::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Bullets = 20;
	GunAttackLength = 7000.0f;
	GunAttackSpreadAngle = 3.0f;
	Damage = 10.0f;
}

void AGun::FireParticleOnMuzzle_Server_Implementation()
{
	FireParticleOnMuzzle_Multicast();
}

void AGun::FireParticleOnMuzzle_Multicast_Implementation()
{
	UGameplayStatics::SpawnEmitterAttached(FireParticle,
		SM_Weapon,
		MuzzleSocket,
		FVector(0, 0, 0),
		FRotator(-90.0f, 0, 0),
		FVector(0.2f)
		);
}


// called on client
FPointDamageEvent AGun::Hit(APlayerCharacter* Causer)
{
	FHitResult HitResult;
	FPointDamageEvent DamageEvent;
	auto Param_IgnoreSelf = FCollisionQueryParams::DefaultQueryParam;
	Param_IgnoreSelf.AddIgnoredActor(Causer);
	Param_IgnoreSelf.bTraceComplex = true;
	
	if(!CanFire(Causer)) { return DamageEvent;}
	Bullets--; MulticastBulltes(Bullets);
	MSB_LOG(Warning, TEXT("current bullets : %d"), Bullets);

	FireParticleOnMuzzle_Server();
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
			auto Character = Cast<APlayerCharacter>(HitResult.GetActor());
			if(Character->IsSameTeam(Causer)) bHit = false;
			else
			{
				bHit = true;
				if(HitResult.BoneName.IsEqual(TEXT("head"))) Damage *= 1.5f;

				auto ToThis = this->GetActorLocation() - HitResult.Location; ToThis.Normalize();
				FPointDamageEvent PointDamageEvent;
				PointDamageEvent.Damage = this->Damage;
				PointDamageEvent.HitInfo = HitResult;
				PointDamageEvent.ShotDirection = ToThis;
				
				//Character->TakeDamage(Damage, PointDamageEvent, this->GetInstigatorController(), Causer);
				DamageEvent = PointDamageEvent;
			}
			
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

	return DamageEvent;
}

bool AGun::CanFire(APlayerCharacter* Causer)
{
	if(Bullets <= 0) {Causer->StopShooting_Server(); return false;}
	return true;
}

void AGun::FillBullets()
{
	Bullets = 45; MulticastBulltes(Bullets);
}

void AGun::MulticastBulltes_Implementation(int32 ClientBullets)
{
	Bullets = ClientBullets;
}
