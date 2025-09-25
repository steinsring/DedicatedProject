// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include <Item/PE_ItemDataTable.h>
#include "PE_FuelWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UPE_FuelWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	int32 MaxQuantity = 10000;
	int32 CurrentQuantity = 0;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UProgressBar* FuelProgressBar;

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	class UDataTable* ItemDataTable;

	void FindItemData(FName ItemID);

	TArray<FPE_ItemDataTable*> ItemDataRows;
	struct FPE_ItemDataTable* SearchedItemData;

	FORCEINLINE FPE_ItemDataTable* GetItemData() const { return SearchedItemData; }

protected:
	virtual void NativeConstruct() override;


public:
	void AddFuel(FName ItemID);
	void UseFuel(int32 Quantity);
	
	int32 GetCurrentFuel() const { return CurrentQuantity; }
};
