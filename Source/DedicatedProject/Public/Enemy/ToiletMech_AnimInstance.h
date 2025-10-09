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
	//virtual void PlayStunMontage(float Duration = 1.0f) override;
	//virtual void EndStunMontage() override;
	
private:

};
