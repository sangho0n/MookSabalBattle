// Fill out your copyright notice in the Description page of Project Settings.


#include "Melee.h"
#include "Engine/DamageEvents.h"

AMelee::AMelee() : Super()
{

}

void AMelee::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

TArray<FHitResult> AMelee::Hit(AActor* Causer)
{
	TArray<FHitResult> HitResults;
	auto Param_IgnoreSelf = FCollisionQueryParams::DefaultQueryParam;
	Param_IgnoreSelf.AddIgnoredActor(Causer);

	auto CauserLocation = Causer->GetActorLocation();
	auto CauserForwardVector = Causer->GetActorForwardVector();
	auto bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		CauserLocation,
		CauserLocation + CauserForwardVector * (this->AttackCapsuleColliderHalfHeight + this->AttackCapsuleColliderRadius) * 2,
		FRotationMatrix::MakeFromZ(CauserForwardVector).ToQuat(),
		ECollisionChannel::ECC_GameTraceChannel4,
		FCollisionShape::MakeCapsule(this->AttackCapsuleColliderRadius, this->AttackCapsuleColliderHalfHeight));
		
	if(bResult)
	{
		TArray<APlayerCharacter*> AlreadyHitActors;
		for(auto res: HitResults)
		{
			if(!res.GetActor()->IsA(APlayerCharacter::StaticClass())) continue;

			auto Character = Cast<APlayerCharacter>(res.GetActor());
			if(AlreadyHitActors.Contains(Character)) continue;
			
			AlreadyHitActors.Push(Character);
			auto ToThis = CauserLocation - res.Location; ToThis.Normalize();
			FPointDamageEvent PointDamageEvent;
			PointDamageEvent.Damage = this->Damage;
			PointDamageEvent.HitInfo = res;
			PointDamageEvent.ShotDirection = ToThis;

			MSB_LOG(Warning, TEXT("attack %s with Pick"), *res.GetActor()->GetName());
			Character->TakeDamage(this->Damage, PointDamageEvent, Causer->GetInstigatorController(), Causer);
		}
	}

#if ENABLE_DRAW_DEBUG
	DrawDebugCapsule(
		GetWorld(),
		CauserLocation + CauserForwardVector * (this->AttackCapsuleColliderHalfHeight + this->AttackCapsuleColliderRadius),
		this->AttackCapsuleColliderHalfHeight,
		this->AttackCapsuleColliderRadius,
		FRotationMatrix::MakeFromZ(CauserForwardVector).ToQuat(),
		bResult ? FColor::Red : FColor::Green,
		false,
		1.0f
	);
#endif

	return HitResults;
}

