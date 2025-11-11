// Fill out your copyright notice in the Description page of Project Settings.


#include "PE_GameState.h"
#include "Player/ProjectPlayer.h"
#include "Net/UnrealNetwork.h"

#include "DedicatedProject.h"

void APE_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APE_GameState, AlivePlayers);
	DOREPLIFETIME(APE_GameState, DeadPlayers);
	DOREPLIFETIME(APE_GameState, DeadPlayerStates);
}

void APE_GameState::AddAlivePlayer(AProjectPlayer* NewPlayer)
{
	// 플레이어가 유효하고 아직 생존자 목록에 없다면 추가
	if (NewPlayer && !AlivePlayers.Contains(NewPlayer))
	{
		AlivePlayers.Add(NewPlayer);
	}
}

void APE_GameState::RemoveAlivePlayer(AProjectPlayer* DeadPlayer)
{
	// 플레이어가 유효하고 생존자 목록에 있다면 제거
	if (DeadPlayer && AlivePlayers.Contains(DeadPlayer))
	{
		AlivePlayers.Remove(DeadPlayer);
	}
}

void APE_GameState::AddDeadPlayer(AProjectPlayer* NewDeadPlayer)
{
	// 플레이어가 유효하고 아직 사망자 목록에 없다면 추가
	if (NewDeadPlayer && !DeadPlayers.Contains(NewDeadPlayer))
	{
		DeadPlayers.Add(NewDeadPlayer);
		DeadPlayerStates.Add(NewDeadPlayer->GetPlayerState());
	}
}

void APE_GameState::RemoveDeadPlayer(AProjectPlayer* RevivedPlayer)
{
	// 플레이어가 유효하고 사망자 목록에 있다면 제거
	if (RevivedPlayer && DeadPlayers.Contains(RevivedPlayer))
	{
		DeadPlayers.Remove(RevivedPlayer);
		DeadPlayerStates.Remove(RevivedPlayer->GetPlayerState());
	}
}

AProjectPlayer* APE_GameState::GetNextAlivePlayer(AProjectPlayer* CurrentPlayer)
{
	if (AlivePlayers.Num() == 0)
		return nullptr;

	int32 CurrentIndex = AlivePlayers.IndexOfByKey(CurrentPlayer);
	int32 NextIndex = (CurrentIndex + 1) % AlivePlayers.Num();
	return AlivePlayers[NextIndex];
}
