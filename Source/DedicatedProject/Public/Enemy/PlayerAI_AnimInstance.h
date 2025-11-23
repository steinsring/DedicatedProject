// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy_AnimInstance.h"
#include "PlayerAI_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UPlayerAI_AnimInstance : public UEnemy_AnimInstance
{
	GENERATED_BODY()
	
public:
	UPlayerAI_AnimInstance();
	
	UFUNCTION()
	void AnimNotify_PlayerAIShoot();
};
