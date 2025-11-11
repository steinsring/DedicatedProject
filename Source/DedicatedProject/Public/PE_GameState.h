// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "PE_GameState.generated.h"

class AProjectPlayer;

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API APE_GameState : public AGameState
{
	GENERATED_BODY()
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UPROPERTY(Replicated)
	TArray<AProjectPlayer*> AlivePlayers;
	
	UPROPERTY(Replicated)
	TArray<AProjectPlayer*> DeadPlayers;

	UPROPERTY(Replicated)
	TArray<APlayerState*> DeadPlayerStates;

	void AddAlivePlayer(AProjectPlayer* NewPlayer);
	void RemoveAlivePlayer(AProjectPlayer* DeadPlayer);

	void AddDeadPlayer(AProjectPlayer* NewDeadPlayer);
	void RemoveDeadPlayer(AProjectPlayer* RevivedPlayer);

	AProjectPlayer* GetNextAlivePlayer(AProjectPlayer* CurrentPlayer);
};
