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

public:
	UFUNCTION(BlueprintPure)
	bool SetIntended(bool isIntended);

private:
	UAnimMontage* ComboMontage;

public:
	void SetBeforeDirection(float NewBeforeDir);
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	bool bIsCW;
};
