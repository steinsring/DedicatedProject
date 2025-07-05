// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FInventorySlot.generated.h"

class UPE_InventoryItemBase;

USTRUCT()
struct FInventorySlot
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<class UPE_InventoryItemBase> Item;

    UPROPERTY()
    int32 Quantity;
};
