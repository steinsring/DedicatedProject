// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PE_LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API APE_LobbyGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	APE_LobbyGameMode();
	
	UFUNCTION(BlueprintCallable, Category = "GameStart")
	void StartGameTravel();

protected:
	virtual void PostLogin(APlayerController* NewPlayer);
};
