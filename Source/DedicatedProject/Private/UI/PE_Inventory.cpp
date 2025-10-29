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

	// 인벤토리 위젯 초기화 ---------------------------------------------------------------------------------
	ItemSlots.Reset();
	ItemSlots.SetNum(MaxInventorySlotNumber);
	ItemSlots[0] = InventorySlot_0;
	ItemSlots[1] = InventorySlot_1;
	ItemSlots[2] = InventorySlot_2;
	ItemSlots[3] = InventorySlot_3;
	ItemSlots[4] = InventorySlot_4;
	ItemSlots[5] = InventorySlot_5;
	/*
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
	*/
}

void UPE_Inventory::SetInventoryData(const TArray<FItemData>& ServerInventoryData)
{
	if (ServerInventoryData.IsEmpty()) 
	{
		PRINT_ERROR_LOG(TEXT("ServerInventoryData Is Empty"));
		return;
	}
	if (ItemSlots.IsEmpty())
	{
		PRINT_LOG(TEXT("ItemSlots is empty!"));
	}

	for (int32 i = 0; i < ItemSlots.Num(); ++i)
	{
		if (!IsValid(ItemSlots[i]))
		{
			PRINT_ERROR_LOG(TEXT("ItemSlots[%d] is null or invalid!"), i);
		}
	}
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