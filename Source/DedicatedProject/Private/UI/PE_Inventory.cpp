// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PE_Inventory.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Overlay.h"
#include "UI/PE_InventorySlot.h"
#include "DedicatedProject.h"
#include "Inventory/FItemData.h"

void UPE_Inventory::NativeConstruct()
{
	Super::NativeConstruct();

	// 데이터 데이블 초기화 --------------------------------------------------------------------------------
	ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/DataTable/DT_ItemDataTable.DT_ItemDataTable"));

	if (ItemDataTable)									//데이터 테이블이 참조되었는지 확인
	{
		ItemDataTable->GetAllRows<FPE_ItemDataTable>(TEXT("PE_Inventory"), ItemDataRows); //테이블의 모든 행을 지역배열로 가져온다.
	}
	else
	{
		PRINT_LOG(TEXT("ItemDataTable is NULL"));
	}

	// 인벤토리 위젯 초기화 ---------------------------------------------------------------------------------
	ItemSlots.Empty();

	int32 Count = ItemGridPanel->GetChildrenCount();
	for (int32 i = 0; i < Count; ++i)
	{
		if (UOverlay* InventorySlotOverlay = Cast<UOverlay>(ItemGridPanel->GetChildAt(i)))
		{
			const int32 ChildCount = InventorySlotOverlay->GetChildrenCount();
			for (int32 j = 0; j < ChildCount; ++j)
			{
				if (UPE_InventorySlot* CheckingSlot = Cast<UPE_InventorySlot>(InventorySlotOverlay->GetChildAt(j)))
				{
					ItemSlots.Add(CheckingSlot);
				}
			}
		}
	}
	// 이거 어따 쓰는거지..
	UClass* LoadedWidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/BluePrints/UI/WB_InventorySlot.WB_InventorySlot_C"));
	if (LoadedWidgetClass)
	{
		InventoryWidget = LoadedWidgetClass;
	}
	else
	{
		PRINT_LOG(TEXT("InventoryWidget is NULL"));
	}
}

FPE_ItemDataTable* UPE_Inventory::FindItemData(FName ItemID)
{
	SearchedItemData = nullptr;						// 이전결과 초기화

	for (FPE_ItemDataTable* Row : ItemDataRows)		// ItemDataTable에서 아이템 정보 검색
	{
		if (Row->ItemID == ItemID)
		{
			return Row;
		}
	}
	return nullptr;
}

void UPE_Inventory::SetInventoryData(const TArray<FItemData> ServerInventoryData)
{
	for (int32 i = 0; i < ItemSlots.Num(); i++)
	{
		ItemSlots[i]->SetItem(ServerInventoryData[i].ItemID, ServerInventoryData[i].Quantity);
	}
}

// 숫자키를 누르면 해당되는 아이템 슬롯을 선택해 표시
void UPE_Inventory::ItemSlotSelect(const int KeyboardNumber)
{
	// 선택된 아이템 슬롯 하이라이트
	if (UUniformGridSlot* GridSlot = Cast<UUniformGridSlot>(Highlight->Slot))
	{
		GridSlot->SetColumn(KeyboardNumber);
	}
}

// 선택된 슬롯에 아이템이 있는지 검사
bool UPE_Inventory::IsEmptySlot(const int SlotNumber) const
{
	if (UPE_InventorySlot* CheckingSlot = ItemSlots[SlotNumber])
	{
		if (CheckingSlot->GetSlotInformation().IsNone())
		{
			return true;
		}
	}
	return false;
}

void UPE_Inventory::UseItem(const int SlotNumber)
{
	if (UPE_InventorySlot* CheckingSlot = ItemSlots[SlotNumber])
	{
		CheckingSlot->UseItem();
		return;
	}
}