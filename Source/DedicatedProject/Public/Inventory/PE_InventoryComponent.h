// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

	// 인벤토리 슬롯 지정을 위한 입력------------------------------------------------------
	UPROPERTY(VisibleAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InventoryMappingContext;					

	static constexpr int32 NumItemSlots = 6;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ItemSlotActions[NumItemSlots];

	void ItemSlotSelect(const FInputActionInstance& Instance);		// 키보트 숫자를 눌렀을때 바인딩 된 함수
	int32 SelectedSlotNumber = 0;

	// 연결된 Inventroy UI ------------------------------------------------------
	UPROPERTY(VisibleAnywhere, Category = "ItemThrowable")
	TObjectPtr<class UPE_ItemThrowableComponent> ItemThrowableComponent;	// 던지는 아이템 컴포넌트

	// 연결된 플레이어 클래스------------------------------------------------------
	UPROPERTY(Transient)
	TWeakObjectPtr<class APE_PlayerController> CachedPC; // 로컬 컨트롤러만 가리키도록

public:	
	// Sets default values for this component's properties
	UPE_InventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// 인벤토리 UI 변경 함수 ------------------------------------------------------------------------------
	UFUNCTION()
	bool AddItem(FName ItemID, int32 ItemQuantity, class AProjectPlayer* Interactor);													// 인벤토리에 아이템을 추가

	UFUNCTION()
	bool RemoveItem(UPE_InventoryItemBase* Item);								// 인벤토리에서 아이템을 제거

	void UseItem(AProjectPlayer* Interactor);									// 인벤토리에서 아이템을 사용

	void SpawnItem();

	void SetInventoryUI(const TArray<struct FItemData> ServerInventoryData);	// 서버에서 인벤토리 UI 세팅

	// 기본 변수 세팅 함수 ------------------------------------------------------------------------------
	void SetItemThrowableComponent(const TObjectPtr<class UPE_ItemThrowableComponent> ThrowableComponent) { ItemThrowableComponent = ThrowableComponent; }

	void InitInputAction(const TObjectPtr<class APE_PlayerController> PlayerController);
};
