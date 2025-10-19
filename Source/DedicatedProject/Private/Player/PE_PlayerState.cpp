// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PE_PlayerState.h"
#include "Inventory/FItemData.h"
#include "DedicatedProject.h"

void APE_PlayerState::InitializeDefaultData()
{
	check(HasAuthority());	//서버 전용

	// 인벤토리 데이터 초기화 ----------------------------------------
	InventoryData.Reset();
	InventoryData.SetNum(MaxInventorySlotNumber);

	for (FItemData& ItemData : InventoryData)
	{
		ItemData.ItemID = NAME_None;
		ItemData.Quantity = 0;
	}

	ForceNetUpdate();	// 서버에서 복제되는 프로퍼티를 바꾼 직후 다음 넷 업데이트 사이클에 강제 전송
}

void APE_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 인벤토리는 보통 본인에게만 복제
	//DOREPLIFETIME_CONDITION(APE_PlayerState, InventoryData, COND_OwnerOnly);
}

// 인벤토리 데이터가 변경될경우 자동으로 호출
void APE_PlayerState::OnRep_InventoryData()
{

}

bool APE_PlayerState::IsEmptySlot(const int32 SlotNumber)
{
	check(HasAuthority());	//서버 전용

	if (InventoryData[SlotNumber].ItemID == NAME_None)
	{
		PRINT_LOG(TEXT("Slot is Empty"));
		return true;
	}

	return false;
}

void APE_PlayerState::UseItem(const int32 SlotNumber)
{
	check(HasAuthority());	//서버 전용
	if (IsEmptySlot(SlotNumber))	return;

	InventoryData[SlotNumber].Quantity -= 1;

	// 개수가 0이 될경우 초기화
	if (InventoryData[SlotNumber].Quantity <= 0)
	{
		InventoryData[SlotNumber].ItemID = NAME_None;
		InventoryData[SlotNumber].Quantity = 0;
	}
}

