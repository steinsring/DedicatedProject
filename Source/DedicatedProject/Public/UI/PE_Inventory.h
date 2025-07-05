// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PE_Inventory.generated.h"

struct FInventorySlot;

UCLASS()
class DEDICATEDPROJECT_API UPE_Inventory : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void UpdateInventoryUI(const TArray<FInventorySlot>& Slots, int32 SelectedIndex);
};
