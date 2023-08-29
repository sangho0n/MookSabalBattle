// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "PlayerCharacter.h"
#include "Animation/AnimInstance.h"
#include "MSBAnimInstance.generated.h"


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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Pawn, meta=(AllowPrivateAccess=true))
	float BeforeDirection; // World Space
	
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

	APlayerCharacter* OwnedPawn;

public:
	UFUNCTION(BlueprintPure)
	bool SetIntended(bool isIntended);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attack, meta=(AllowPrivateAccess=true))
	UAnimMontage* ComboMontage;

public:
	void SetBeforeDirection(float NewBeforeDir);
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	bool bIsCW;

public:
	UFUNCTION()
	void OnComboMontageEnded(UAnimMontage* montage, bool bInterrupted);

	UFUNCTION()
	void AnimNotify_HitCheck();
	UFUNCTION()
	void AnimNotify_NextComboCheck();
	UFUNCTION()
	void AnimNotify_FinalHitCheck();

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
};
