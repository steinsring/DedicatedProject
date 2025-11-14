// Fill out your copyright notice in the Description page of Project Settings.


#include "PE_GameState.h"
#include "Player/ProjectPlayer.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"

#include "DedicatedProject.h"

void APE_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APE_GameState, AlivePlayers);
	DOREPLIFETIME(APE_GameState, DeadPlayers);
}

void APE_GameState::AddAlivePlayer(APlayerState* NewPlayerState)
{
	// 플레이어가 유효하고 아직 생존자 목록에 없다면 추가
	if (NewPlayerState && !AlivePlayers.Contains(NewPlayerState))
	{
		AlivePlayers.Add(NewPlayerState);
	}
}

void APE_GameState::RemoveAlivePlayer(APlayerState* DeadPlayerState)
{
	// 플레이어가 유효하고 생존자 목록에 있다면 제거
	if (DeadPlayerState && AlivePlayers.Contains(DeadPlayerState))
	{
		AlivePlayers.Remove(DeadPlayerState);
	}
}

void APE_GameState::AddDeadPlayer(APlayerState* NewDeadPlayerState)
{
	// 플레이어가 유효하고 아직 사망자 목록에 없다면 추가
	if (NewDeadPlayerState && !DeadPlayers.Contains(NewDeadPlayerState))
	{
		DeadPlayers.Add(NewDeadPlayerState);
	}
}

void APE_GameState::RemoveDeadPlayer(APlayerState* RevivedPlayerState)
{
	// 플레이어가 유효하고 사망자 목록에 있다면 제거
	if (RevivedPlayerState && DeadPlayers.Contains(RevivedPlayerState))
	{
		DeadPlayers.Remove(RevivedPlayerState);
	}
}

AProjectPlayer* APE_GameState::GetNextAlivePlayer(AProjectPlayer* CurrentPlayer)
{
	// 0. 비어 있으면 바로 종료
	if (AlivePlayers.Num() == 0)
	{
		PRINT_LOG(TEXT("GetNextAlivePlayer: AlivePlayers is EMPTY"));
		return nullptr;
	}

	// 1. 현재 GameState 인스턴스 / 배열 상태 디버그 출력
	PRINT_LOG(TEXT("GetNextAlivePlayer: This = %p, AliveNum = %d"), this, AlivePlayers.Num());

	for (int32 i = 0; i < AlivePlayers.Num(); ++i)
	{
		APlayerState* PS = AlivePlayers[i];
		AProjectPlayer* P = PS ? Cast<AProjectPlayer>(PS->GetPawn()) : nullptr;
		if (P)
		{
			PRINT_LOG(TEXT("  AlivePlayers[%d] = %s (IsDead=%d)"),
				i, *P->GetName(), P->GetIsDead());
		}
		else
		{
			PRINT_LOG(TEXT("  AlivePlayers[%d] = NULL"), i);
		}
	}

	// 2. CurrentPlayer가 배열 안에서 몇 번째인지 찾기
	int32 CurrentIndex = INDEX_NONE;
	APlayerState* CurrentPS = CurrentPlayer ? CurrentPlayer->GetPlayerState<APlayerState>() : nullptr;
	if (CurrentPS)
	{
		CurrentIndex = AlivePlayers.IndexOfByKey(CurrentPS);
	}

	PRINT_LOG(TEXT("GetNextAlivePlayer: CurrentPlayer = %s, CurrentIndex = %d"),
		CurrentPlayer ? *CurrentPlayer->GetName() : TEXT("NULL"),
		CurrentIndex);

	const int32 Num = AlivePlayers.Num();

	// 3. 못 찾았으면 -1에서 시작해서 +1 => 0부터 보게 만들기
	if (CurrentIndex == INDEX_NONE)
	{
		CurrentIndex = -1;
	}

	// 4. 한 바퀴 돌면서 "유효하고 안 죽은" 플레이어 찾기
	for (int32 Offset = 1; Offset <= Num; ++Offset)
	{
		const int32 NextIndex = (CurrentIndex + Offset) % Num;
		APlayerState* NextPS = AlivePlayers[NextIndex];
		if (!NextPS)
		{
			PRINT_LOG(TEXT("GetNextAlivePlayer: AlivePlayers[%d] is NULL"), NextIndex);
			continue;
		}

		AProjectPlayer* Candidate = Cast<AProjectPlayer>(NextPS->GetPawn());

		if (Candidate && !Candidate->GetIsDead())
		{
			PRINT_LOG(TEXT("GetNextAlivePlayer: Choose index %d (%s)"),
				NextIndex, *Candidate->GetName());
			return Candidate;
		}
	}

	// 5. 여기까지 왔다는 건 AlivePlayers 안에 유효한 Pawn이 없다는 뜻
	PRINT_LOG(TEXT("GetNextAlivePlayer: No valid candidate found (all null or dead)"));
	return nullptr;
}
