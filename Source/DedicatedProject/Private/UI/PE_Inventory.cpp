// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PE_Inventory.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Overlay.h"
#include "Components/VerticalBox.h"
#include "UI/PE_InventorySlot.h"
#include "DedicatedProject.h"
#include "Inventory/FItemData.h"
#include "UI/PE_NotifyWindow.h"
#include "PE_GameInstance.h"

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

	if (NotifyWindow)
	{
		NotifyWindow->ClearChildren();
	}
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

	// GameInstance 값 가져와서 스테이지 UI 세팅
	if (UWorld* World = GetWorld())
	{
		if (UPE_GameInstance* GI = World->GetGameInstance<UPE_GameInstance>())
		{
			const int32 CurrentLevel = GI->GetCurrentLevelCount();
			const int32 MaxLevel = GI->GetMaxTravelLevelCount();

			if (CurrentStage)
			{
				CurrentStage->SetText(FText::AsNumber(CurrentLevel));
			}

			if (MaxStage)
			{
				MaxStage->SetText(FText::AsNumber(MaxLevel));
			}
		}
	}
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

void UPE_Inventory::CreateNotify(UPE_NotifyWindow* NotifyWidget)
{
	if (!NotifyWindow || !NotifyWidget) return;

	// 2초 뒤 위젯 자동 삭제 (Weak 포인터로 안전하게 잡기)
	TWeakObjectPtr<UPE_NotifyWindow> WeakNotifyWidget = NotifyWidget;

	if (UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateLambda([WeakNotifyWidget]()
				{
					if (WeakNotifyWidget.IsValid())
					{
						WeakNotifyWidget->RemoveFromParent();
					}
				}),
			2.0f,
			false
		);
	}

	// 🔴 여기서부터는 "최대 3개 유지" 처리

	int32 ChildCount = NotifyWindow->GetChildrenCount();

	if (ChildCount >= 3)
	{
		int32 LastIndex = ChildCount - 1;       // 🔹 마지막 위젯
		UWidget* OldWidget = NotifyWindow->GetChildAt(LastIndex);

		if (OldWidget)
		{
			NotifyWindow->RemoveChild(OldWidget);
		}
	}
	NotifyWindow->AddChild(NotifyWidget);
}