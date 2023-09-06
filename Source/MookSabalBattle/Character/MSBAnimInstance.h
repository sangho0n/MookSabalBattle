// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "PlayerCharacter.h"
#include "Animation/AnimInstance.h"
#include "MSBAnimInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitCheck, int32, CurrCombo);
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
	TEnumAsByte<CharacterMode> CurrentMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	bool bIsAttacking;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	float MaxWalkSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	float DeltaYaw;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	float Pitch;

	APlayerCharacter* OwnedCharacter;

public:
	UFUNCTION(BlueprintPure)
	bool SetIntended(bool isIntended);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attack, meta=(AllowPrivateAccess=true))
	UAnimMontage* ComboMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	bool bIsCW;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attack, meta=(AllowPrivateAccess=true))
	int32 RandomMeleeIdx;

public:
	UFUNCTION()
	void OnComboMontageEnded(UAnimMontage* montage, bool bInterrupted);

	UFUNCTION()
	void AnimNotify_HitCheck();
	UFUNCTION()
	void AnimNotify_NextComboCheck();

private:
	int32 CurrentCombo;
	const int32 MaxCombo = 3;
	bool CanNextCombo;
	bool NextComboInputOn;
	FName GetNextComboSectionName();

public:
	bool GetCanNextCombo()
	{
		return CanNextCombo;
	}
	void SetNextComboInputOn(bool flag)
	{
		NextComboInputOn = flag;
	}

	void JumpToNextSection();

	void PlayMeleeSwing();

	UFUNCTION(BlueprintCallable, Category=Attack)
	void SetSwingEnd();

	void PlayShot();

private:
	FOnHitCheck OnHitCheck;
};
