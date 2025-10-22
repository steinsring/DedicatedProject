// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Item/PE_ItemDataTable.h>
#include "PE_InventorySlot.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class DEDICATEDPROJECT_API UPE_InventorySlot : public UUserWidget
{
	GENERATED_BODY()

private:
	int32 Stack = 0;
	FName ID;

	UFUNCTION()
	void FindItemData(FName ItemID);

	TArray<FPE_ItemDataTable*> ItemDataRows;
	struct FPE_ItemDataTable* SearchedItemData;

	FORCEINLINE FPE_ItemDataTable* GetItemData() const { return SearchedItemData; }
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
	class UDataTable* ItemDataTable;
	
public:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UImage* IconImage;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* Number;

	void SetItem(FName ItemID, int32 ItemQuantity);

	FName GetSlotInformation();

	void UseItem();
};
