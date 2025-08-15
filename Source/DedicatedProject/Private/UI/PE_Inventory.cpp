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
		if (UOverlay* InventorySlot = Cast<UOverlay>(ItemGridPanel->GetChildAt(i)))
		{
			InventorySlots.Add(InventorySlot);
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

void UPE_Inventory::AddItemToInventory(UTexture2D* ItemIcon, int32 Quantity)
{
	PRINT_LOG(TEXT("Try Add Item To Inventory"));
	/*
	APlayerController* LocalPlayerController = GetWorld()->GetFirstPlayerController();
	UPE_InventorySlot* NewSlot = CreateWidget<UPE_InventorySlot>(LocalPlayerController, InventoryWidget);
	NewSlot->SetItem(ItemIcon, Quantity);
	InventorySlots[0]->AddChild(NewSlot);
	*/
	UPE_InventorySlot* FoundSlot = nullptr;
	const int32 ChildCount = InventorySlots[0]->GetChildrenCount();
	for (int32 i = 0; i < ChildCount; ++i)
	{
		if (UPE_InventorySlot* Temp = Cast<UPE_InventorySlot>(InventorySlots[0]->GetChildAt(i)))
		{
			FoundSlot = Temp;
			break; // 첫 번째만 찾고 끝내려면 break
		}
	}

	if (FoundSlot)
	{
		FoundSlot->SetItem(ItemIcon, Quantity);
	}
	//PRINT_LOG(TEXT("InventorySlots is valid. Name: %s"), *InventorySlots[0]->GetName());
	//PRINT_LOG(TEXT("NewSlot is valid. Name: %s"), *NewSlot->GetName());
}

// 숫자키를 누르면 해당되는 아이템 슬롯을 선택해 표시하고 해당 아이템 정보를 반환
void UPE_Inventory::ItemSlotSelect(int KeyboardNumber)
{
	// 선택된 아이템 슬롯 하이라이트
	if (UUniformGridSlot* GridSlot = Cast<UUniformGridSlot>(Highlight->Slot))
	{
		GridSlot->SetColumn(KeyboardNumber);
	}


}