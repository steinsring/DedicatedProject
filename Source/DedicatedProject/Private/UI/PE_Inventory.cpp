// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PE_Inventory.h"
#include "Components/UniformGridPanel.h"
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

void UPE_Inventory::AddItemToInventory(UTexture2D* ItemIcon)
{
	PRINT_LOG(TEXT("Try Add Item To Inventory"));
	APlayerController* LocalPlayerController = GetWorld()->GetFirstPlayerController();
	UPE_InventorySlot* NewSlot = CreateWidget<UPE_InventorySlot>(LocalPlayerController, InventoryWidget);
	NewSlot->SetIcon(ItemIcon);
	InventorySlots[0]->AddChild(NewSlot);
	PRINT_LOG(TEXT("InventorySlots is valid. Name: %s"), *InventorySlots[0]->GetName());
	PRINT_LOG(TEXT("NewSlot is valid. Name: %s"), *NewSlot->GetName());
}