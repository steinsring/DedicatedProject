// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PE_Inventory.generated.h"

class UUniformGridPanel;
class UOverlay;
class UPE_InventorySlot;

UCLASS()
class DEDICATEDPROJECT_API UPE_Inventory : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<UOverlay*> InventorySlots;				// 인벤토리 슬롯 배열

	UPROPERTY(VisibleAnywhere, Category = UI)
	TSubclassOf<UPE_InventorySlot> InventoryWidget;

protected:
	virtual void NativeConstruct() override;
	
public:
	// 블루프린트에서 바인딩할 UniformGridPanel
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> ItemGridPanel = nullptr;

	// 아이템 추가 함수
	void AddItemToInventory(UTexture2D* ItemIcon);
};
