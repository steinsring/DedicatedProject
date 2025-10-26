// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FItemData.generated.h"

USTRUCT()
struct FItemData
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FName ItemID = NAME_None;

	UPROPERTY()
	int32 Quantity = 0;
};