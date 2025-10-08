// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PE_SkillDataTable.generated.h"

USTRUCT(BlueprintType)
struct DEDICATEDPROJECT_API FPE_SkillDataTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FName SkillID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FText SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	bool bIsUnlocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	bool bIsSelected = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	float Cost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	float CoolTime;
};
