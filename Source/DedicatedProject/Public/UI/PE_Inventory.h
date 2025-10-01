// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Item/PE_ItemDataTable.h>
#include "PE_Inventory.generated.h"

class UPE_InventorySlot;

UCLASS()
class DEDICATEDPROJECT_API UPE_Inventory : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<UPE_InventorySlot*> ItemSlots;				// 인벤토리 슬롯 배열

	UPROPERTY(VisibleAnywhere, Category = UI)
	TSubclassOf<UPE_InventorySlot> InventoryWidget;			

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	class UDataTable* ItemDataTable;						// 잠조할 아이템 데이터 테이블

	TArray<FPE_ItemDataTable*> ItemDataRows;				// 데이터 테이블 내용
	struct FPE_ItemDataTable* SearchedItemData;				// 찾은 데이터

protected:
	virtual void NativeConstruct() override;
	
public:
	// 블루프린트에서 바인딩할 UniformGridPanel
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> ItemGridPanel = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidget> Highlight = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPE_InventorySlot> LightSlot_1 = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPE_InventorySlot> WeaponSlot_1 = nullptr;

	// 아이템 추가 함수
	void AddItemToInventory(const FName ItemID);

	// 슬롯 선택시 해당 아이템 반환
	void ItemSlotSelect(const int KeyboardNumber);

	bool IsEmptySlot(const int SlotNumber) const;

	void UseItem(const int SlotNumber);

	FPE_ItemDataTable* FindItemData(FName ItemID);
};
