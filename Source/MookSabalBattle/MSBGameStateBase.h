// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MookSabalBattle.h"
#include "GameFramework/GameStateBase.h"
#include "Character/PlayerCharacter.h"
#include "MSBGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreChanged, int32, BlueTeamScore, int32, RedTeamScore);

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API AMSBGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Score, Category=GamePlay)
	int32 BlueTeamScore;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Score, Category=GamePlay)
	int32 RedTeamScore;
	UFUNCTION()
	void OnRep_Score();
public:
	// only called on Server
	void AdjustScore(APlayerCharacter* DeadCharacter);

	UPROPERTY(BlueprintAssignable, Category=GameState)
	FOnScoreChanged OnScoreChanged;
};
