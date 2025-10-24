// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Inventory/FItemData.h"
#include "Inventory/PE_InventoryComponent.h"
#include "PE_PlayerState.generated.h"


UCLASS()
class DEDICATEDPROJECT_API APE_PlayerState : public APlayerState
{
	GENERATED_BODY()
private:
	// 플레이어 인벤토리 관련 서버 권위 데이터 -------------------------------------------------------
	UPROPERTY(ReplicatedUsing = OnRep_InventoryData)	// 값이 변경되면 OnRep_InventoryData()함수 자동 호출
	TArray<FItemData> InventoryData;

	// 인벤토리 관련 기본 변수 ----------------------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<UPE_InventoryComponent> MyInventoryComponent;				// 자신의 인벤토리 컴포넌트

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	int32 MaxInventorySlotNumber = 6;										// 최대 인벤토리 슬롯 개수

	int32 SelectedSlotNumber = 0;											// 현재 선택된 슬롯 번호
	// -----------------------------------------------------------------
	UFUNCTION()                      
	void OnRep_InventoryData();												// 여기서 클라이언트의 UI 갱신
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 초기화 함수 -----------------------------------------------------------------------------
	void InitializeDefaultData();											// Gamemode의 PostLogin에서 호출
	void SetInventoryComponent(const TObjectPtr<UPE_InventoryComponent> Component) { MyInventoryComponent = Component; }	// 플레이어 Character, PossessedBy에서 호출

	// 인벤토리 데이터 설정 함수 -----------------------------------------------------------------------------
	bool IsEmptySlot(const int32 SlotNumber);

	UFUNCTION(Server,Reliable)
	void AddItem_Server(const FName ItemID, const int32 ItemQuantity);					// 아이템 획득 RPC

	void UseItem(const int32 SlotNumber);									// 아이템 사용 RPC
};
