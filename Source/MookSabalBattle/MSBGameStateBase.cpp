// Fill out your copyright notice in the Description page of Project Settings.


#include "MSBGameStateBase.h"

#include "Character/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

void AMSBGameStateBase::BeginPlay()
{
	BlueTeamScore = 0;
	RedTeamScore = 0;
}


void AMSBGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMSBGameStateBase, BlueTeamScore);
	DOREPLIFETIME(AMSBGameStateBase, RedTeamScore);
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
