// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FItemData.generated.h"

USTRUCT()
struct FItemData
{
	GENERATED_BODY()
public:
	FName ItemID = NAME_None;
	int32 Quantity = 0;
};