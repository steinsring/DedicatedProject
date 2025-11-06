// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PE_ToiletMechStats.generated.h"

USTRUCT(BlueprintType)
struct DEDICATEDPROJECT_API FPE_ToiletMechStats : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Speed = 200.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxSpeed = 400.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DamageMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AttackPower = 1.0f;
};
