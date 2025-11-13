// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/FItemData.h"
#include "FPlayerData.generated.h"

USTRUCT()
struct FPlayerData
{
	GENERATED_BODY()

    UPROPERTY()
    TArray<FItemData> InventoryData;
};