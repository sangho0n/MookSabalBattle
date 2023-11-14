// Fill out your copyright notfkice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "PlayerCharacter.h"
#include "Animation/AnimInstance.h"
#include "MookSabalBattle/Weapon/Melee.h"
#include "MSBAnimInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitCheck, int32, CurrCombo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOverDeltaOffset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadAnimEnd);
/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API UMSBAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UMSBAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeBeginPlay() override;

	virtual void PostInitProperties() override;

	void PlayComboAnim();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	float CurrentPawnSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Pawn, meta=(AllowPrivateAccess=true))
	float MovingDirection; // Obj Space
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	bool bInAir;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	bool bIsIntended;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	TEnumAsByte<CharacterMode> CurrentMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	bool bIsAttacking;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	float MaxWalkSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	float DeltaYaw;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	float Pitch;

	UPROPERTY(BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	APlayerCharacter* OwnedCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	bool bIsReload;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attack, meta=(AllowPrivateAccess=true))
	UAnimMontage* ComboMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	bool bIsCW;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attack, meta=(AllowPrivateAccess=true))
	int32 RandomMeleeIdx;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	int32 RandomDeadIdx3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	int32 RandomDeadIdx2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attack, meta=(AllowPrivateAccess=true))
	AMelee* CurrentMelee;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attack, meta=(AllowPrivateAccess=true))
	float SwingPlayRate;

	int32 CurrentCombo;

	const int32 MaxCombo = 3;

	bool NextComboInputOn;

	FName GetNextComboSectionName();
public:
	UFUNCTION(BlueprintPure)
	bool SetIntended(bool isIntended);

	UFUNCTION()
	void OnComboMontageEnded(UAnimMontage* montage, bool bInterrupted);

	UFUNCTION()
	void AnimNotify_HitCheck();
	
	UFUNCTION()
	void AnimNotify_NextComboCheck();
	
	void SetNextComboInputOn(bool flag)
	{
		NextComboInputOn = flag;
	}

	void JumpToNextSection();

	void PlayRandomMeleeSwing();
	
	void PlayRandomDeadAnim();

	UFUNCTION(BlueprintCallable)
	void ReloadEnd();
	
	UFUNCTION(BlueprintCallable)
	void SwingEnd();

private:
	FOnHitCheck OnHitCheck;

	FOnOverDeltaOffset OnOverDeltaOffset;

	UFUNCTION()
	void ResetDelta();

public:
	FOnReloadAnimEnd OnReloadAnimEnd;

	FOnAttackEnd OnAttackEnd;
};
