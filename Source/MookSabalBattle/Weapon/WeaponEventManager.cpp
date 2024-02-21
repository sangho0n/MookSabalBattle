// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponEventManager.h"

TObjectPtr<UWeaponEventManager> UWeaponEventManager::Instance = nullptr;

UWeaponEventManager::UWeaponEventManager()
{
	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_Particle(TEXT("/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Sparks/P_Sparks_C.P_Sparks_C"));
	if(P_Particle.Succeeded())
	{
		FireParticle = P_Particle.Object;
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> BulletTraceRef(TEXT("/Script/Niagara.NiagaraSystem'/Game/Main/VFX/Bullet/NS_BulletTrail.NS_BulletTrail'"));
	if(BulletTraceRef.Succeeded())
	{
		BulletTrace = BulletTraceRef.Object;
	}
}

TObjectPtr<UWeaponEventManager> UWeaponEventManager::GetInstance()
{
	if(Instance.IsNull())
	{
		Instance = NewObject<UWeaponEventManager>();
		check(Instance);
	}
	return Instance;
}

void UWeaponEventManager::BeginDestroy()
{
	if(!Instance.IsNull())
	{
		Instance = nullptr;
		// Instance will be deleted by GC
	}
	Super::BeginDestroy();
}


void UWeaponEventManager::PostInitProperties()
{
	Super::PostInitProperties();

	OnFire.AddDynamic(this, &UWeaponEventManager::SpawnFireParticle);
}

void UWeaponEventManager::SpawnFireParticle(FVector MuzzlePoint, FVector TargetPoint, bool bIsBlocked, bool bIsHit)
{
	UE_LOG(MookSablBattle, Log, TEXT("dfdf"));
	UGameplayStatics::SpawnEmitterAtLocation(GEngine->GameViewport->GetWorld(),
		FireParticle,
		MuzzlePoint,
		FRotator(-90.0f, 0, 0),
		FVector(0.1f, 1.f, 0.1f)
		);
}
