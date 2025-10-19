// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Inventory/FItemData.h"
#include "PE_PlayerState.generated.h"


UCLASS()
class DEDICATEDPROJECT_API APE_PlayerState : public APlayerState
{
	GENERATED_BODY()
private:
	// 플레이어 인벤토리 관련 서버 권위 데이터 -------------------------------------------------------
	UPROPERTY(ReplicatedUsing = OnRep_InventoryData)	// 값이 변경되면 OnRep_InventoryData()함수 자동 호출
	TArray<FItemData> InventoryData;

	UPROPERTY(VisibleAnywhere)
	int32 MaxInventorySlotNumber = 6;

	UFUNCTION()                      
	void OnRep_InventoryData();			// 여기서 클라이언트의 UI 갱신
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	void InitializeDefaultData();		// Gamemode의 PostLogin에서 호출

	bool IsEmptySlot(const int32 SlotNumber);

	void UseItem(const int32 SlotNumber);		// 아이템 사용 RPC
};
