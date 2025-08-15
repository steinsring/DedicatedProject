// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Item/PE_ItemDataTable.h>
#include "PE_InventoryComponent.generated.h"


class UPE_InventoryItemBase;
class UPE_Inventory;
class UInputMappingContext;
class UInputAction;
struct FInputActionInstance;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEDICATEDPROJECT_API UPE_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	int32 MaxSlots = 4;

	// 인벤토리 위젯 클래스
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TSubclassOf<class UPE_Inventory> InventoryWidgetClass;

	// 실제 생성된 인벤토리 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<UPE_Inventory> InventoryWidget;

	UFUNCTION()
	void FindItemData(FName ItemID);

	TArray<FPE_ItemDataTable*> ItemDataRows;

	struct FPE_ItemDataTable* SearchedItemData;

	// 인벤토리 슬롯 지정을 위한 입력
	UPROPERTY(VisibleAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InventoryMappingContext;					

	static constexpr int32 NumItemSlots = 6;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ItemSlotActions[NumItemSlots];

	void ItemSlotSelect(const FInputActionInstance& Instance);		// 키보트 숫자를 눌렀을때 바인딩 된 함수

public:	
	// Sets default values for this component's properties
	UPE_InventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
	class UDataTable* ItemDataTable;

	FORCEINLINE FPE_ItemDataTable* GetItemData() const { return SearchedItemData; }

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	bool AddItem(FName ItemID);								// 인벤토리에 아이템을 추가

	UFUNCTION()
	bool RemoveItem(UPE_InventoryItemBase* Item);			// 인벤토리에서 아이템을 제거
	
};
