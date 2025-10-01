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

void UPE_Inventory::AddItemToInventory(const FName ItemID)
{
	PRINT_LOG(TEXT("Try Add Item To Inventory"));
	SearchedItemData = FindItemData(ItemID);
	// NULL Check---------------------------------------------------------------------------------
	if (!SearchedItemData)
	{
		PRINT_ERROR_LOG(TEXT("Find Item Fail"));
		return;
	}

	// 아이템을 인벤토리에 추가 --------------------------------------------------------------------
	if (SearchedItemData->ItemTag == "Item")
	{
		UPE_InventorySlot* FoundSlot = nullptr;
		for (UPE_InventorySlot* InvenSlot : ItemSlots)
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
			for (UPE_InventorySlot* InvenSlot : ItemSlots)
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
	// 조명을 인벤토리에 추가 --------------------------------------------------------------------
	else if (SearchedItemData->ItemTag == "Light")
	{

	}
	// 장비를 인벤토리에 추가 --------------------------------------------------------------------
	else if (SearchedItemData->ItemTag == "Weapon")
	{

	}
	// 잘못된 Tag -------------------------------------------------------------------------------
	else
	{
		PRINT_ERROR_LOG(TEXT("Incorrect ItemTag"));
		return;
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