// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Player/ProjectPlayer.h"
#include "PE_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API APE_GameMode : public AGameMode
{
	GENERATED_BODY()

private:
	UPROPERTY()
	FVector TargetLocation;
	bool bHasTarget = false;

	UPROPERTY()
	TArray<TObjectPtr<class APE_PlayerState>> AllPlayerState;
	
public:
	APE_GameMode(); //게임모드 생성자
	void SetPlayerLocation(FVector NewLocation);

	UFUNCTION()
	void PlacePawnIfReady(APlayerController* PC);

	FVector GetTheTargetLocation()	const { return TargetLocation; }
	void SetTargetLocation(FVector Location) { TargetLocation = Location; }
	bool GetbHasTarget()	const { return bHasTarget; }
	void SetbHasTarget(bool bHas) { bHasTarget = bHas; }

	void HandlePlayerDeath(AController* DeadController);
	void HandlePlayerRespawn(APlayerState* DeadPS, AProjectPlayer* TargetDummy);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	//virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};
