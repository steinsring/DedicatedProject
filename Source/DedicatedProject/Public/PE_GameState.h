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
	TArray<APlayerState*> AlivePlayers;
	
	UPROPERTY(Replicated)
	TArray<APlayerState*> DeadPlayers;

	void AddAlivePlayer(APlayerState* NewPlayerState);
	void RemoveAlivePlayer(APlayerState* DeadPlayerState);

	void AddDeadPlayer(APlayerState* NewDeadPlayerState);
	void RemoveDeadPlayer(APlayerState* RevivedPlayerState);

	AProjectPlayer* GetNextAlivePlayer(AProjectPlayer* CurrentPlayer);
};
