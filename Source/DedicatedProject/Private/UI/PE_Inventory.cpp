// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PE_Inventory.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Overlay.h"
#include "UI/PE_InventorySlot.h"
#include "DedicatedProject.h"

void UPE_Inventory::NativeConstruct()
{
	Super::NativeConstruct();

	InventorySlots.Empty();

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
					InventorySlots.Add(CheckingSlot);
				}
			}
		}
	}

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

void UPE_Inventory::AddItemToInventory(const FName ItemID)
{
	PRINT_LOG(TEXT("Try Add Item To Inventory"));
	UPE_InventorySlot* FoundSlot = nullptr;
	for (UPE_InventorySlot* InvenSlot : InventorySlots)
	{
		if (InvenSlot->GetSlotInformation() == ItemID)
		{// 슬롯에 아이템이 같은경우 stack 추가
			FoundSlot = InvenSlot;
			break;
		}
	}

	// FoundSlot이 nullptr인경우 인벤토리안에 습득한 아이템과 같은 아이템이 없어 빈 슬롯에 추가
	if (!FoundSlot)
	{
		for (UPE_InventorySlot* InvenSlot : InventorySlots)
		{
			if (InvenSlot->GetSlotInformation().IsNone())
			{
				FoundSlot = InvenSlot;
				break; // 첫 번째만 찾고 끝내려면 break
			}
		}
	}
	
	if (FoundSlot)
	{
		FoundSlot->SetItem(ItemID);
	}
	else // 모든 인벤토리 슬롯이 가득찬 경우
	{
		PRINT_LOG(TEXT("All Inventory Slot are Full"));
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
	if (UPE_InventorySlot* CheckingSlot = InventorySlots[SlotNumber])
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
	if (UPE_InventorySlot* CheckingSlot = InventorySlots[SlotNumber])
	{
		CheckingSlot->UseItem();
		return;
	}
}