// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SkillManagerComponent.h"
#include "PE_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API APE_PlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TSubclassOf<class UPE_Inventory> InventoryWidgetBPClass;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<class UPE_Inventory> InventoryWidget;				// 실제 생성된 인벤토리 위젯 인스턴스

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<class UPE_InventoryComponent> InventoryComponent;	// 인벤토리 컴포넌트
protected:
	virtual void BeginPlay() override;
	
public:
	APE_PlayerController();

	void InitializeDefaultData();		// Gamemode의 PostLogin에서 호출

	void InitInventoryUI();				// Inventory UI 생성

	void ItemSlotSelect(int32 i);

	void InventoryDataUpdate(const TArray<struct FItemData>& InventoryData);
};
