// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy_AnimInstance.h"
#include "ToiletMech_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UToiletMech_AnimInstance : public UEnemy_AnimInstance
{
	GENERATED_BODY()

public:
	UToiletMech_AnimInstance();
	
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackPattern1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackPattern2;
};
