// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FInventorySlot.h"
#include "PE_InventoryComponent.generated.h"

class UPE_InventoryItemBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEDICATEDPROJECT_API UPE_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<FInventorySlot> Inventory;

	UPROPERTY()
	int32 MaxSlots = 20;

public:	
	// Sets default values for this component's properties
	UPE_InventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	bool AddItem(UPE_InventoryItemBase* Item, int32 Quantity);

	UFUNCTION()
	bool RemoveItem(UPE_InventoryItemBase* Item, int32 Quantity);
	
};
