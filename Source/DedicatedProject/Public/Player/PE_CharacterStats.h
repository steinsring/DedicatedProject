// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PE_CharacterStats.generated.h"

USTRUCT(BlueprintType)
struct DEDICATEDPROJECT_API FPE_CharacterStats : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float WalkSpeed = 200.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SprintSpeed = 400.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DamageMultiplier = 1.0f; //받는 데미지 배율

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 NextLevelXp = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float StealthMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AttackPower = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AttackPowerMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SpeedMultiplier = 1.0f;
};
