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

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	float CurrentPawnSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	float Direction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	bool bInAir;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	bool bIsIntended;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess=true))
	CharacterMode CurrentMode;
};
