// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common_AnimInstance.h"
#include "PE_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UPE_AnimInstance : public UCommon_AnimInstance
{
	GENERATED_BODY()
	
public:
	UPE_AnimInstance();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = true))
	UAnimMontage* BasicAttack;
};
