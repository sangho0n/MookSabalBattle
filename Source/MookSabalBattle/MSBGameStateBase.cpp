// Fill out your copyright notice in the Description page of Project Settings.


#include "MSBGameStateBase.h"

#include "Character/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

AMSBGameStateBase::AMSBGameStateBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bIsGameStarted = false;
	AccGameSeconds = 0;
}

void AMSBGameStateBase::BeginPlay()
{
	BlueTeamScore = 0;
	RedTeamScore = 0;
	bIsGameStarted = false;
	AccGameSeconds = 0;
}


void AMSBGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMSBGameStateBase, BlueTeamScore);
	DOREPLIFETIME(AMSBGameStateBase, RedTeamScore);
	DOREPLIFETIME(AMSBGameStateBase, AccGameSeconds);
}

void AMSBGameStateBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bIsGameStarted)
		AccGameSeconds += DeltaSeconds;
}

float AMSBGameStateBase::GetLeftTimeRatio()
{
	float Ratio = (GamePlaySeconds - AccGameSeconds) / GamePlaySeconds;
	if(Ratio < KINDA_SMALL_NUMBER)
	{
		bIsGameStarted = false;
		OnGameEnd.Broadcast();
	}
	return Ratio;
}

// only called on server
void AMSBGameStateBase::AdjustScore(APlayerCharacter* DeadCharacter)
{
	bool bIsRedDied = DeadCharacter->GetCharacterStateComponent()->IsRedTeam();
	if(bIsRedDied)
		BlueTeamScore++;
	else
		RedTeamScore++;
	
	OnScoreChanged.Broadcast(BlueTeamScore, RedTeamScore);
}

void AMSBGameStateBase::OnRep_Score()
{
	OnScoreChanged.Broadcast(BlueTeamScore, RedTeamScore);
}

void AMSBGameStateBase::PlayGame()
{
	bIsGameStarted = true;
}
