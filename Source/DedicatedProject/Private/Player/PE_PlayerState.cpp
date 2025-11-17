// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PE_PlayerState.h"
#include "Inventory/FItemData.h"
#include "DedicatedProject.h"
#include "Net/UnrealNetwork.h"			//DOREPLIFETIME_CONDITION
#include "Player/PE_PlayerController.h"
#include "Player/ProjectPlayer.h"
#include <Item/PE_ItemDataTable.h>

void APE_PlayerState::InitializeDefaultData(TArray<FItemData> DefualtInventoryData, bool isFirstStage, int32 DefualtFuelData)
{
	check(HasAuthority());	//서버 전용

	// 인벤토리 데이터 초기화 ----------------------------------------
	if (isFirstStage)
	{
		InventoryData.Reset();
		InventoryData.SetNum(MaxInventorySlotNumber);

		for (FItemData& ItemData : InventoryData)
		{
			ItemData.ItemID = NAME_None;
			ItemData.Quantity = 0;
		}
	}
	else
	{
		InventoryData = DefualtInventoryData;
		CurrentQuantity = DefualtFuelData;
	}


	// 데이터 테이블 등록 ----------------------------------------
	ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/DataTable/DT_ItemDataTable.DT_ItemDataTable"));

	if (ItemDataTable)									//데이터 테이블이 참조되었는지 확인
	{
		ItemDataTable->GetAllRows<FPE_ItemDataTable>(TEXT("ProjectPlayer"), ItemDataRows); //테이블의 모든 행을 지역배열로 가져온다.
	}
	else
	{
		PRINT_LOG(TEXT("ItemDataTable is NULL"));
	}



	ForceNetUpdate();	// 서버에서 복제되는 프로퍼티를 바꾼 직후 다음 넷 업데이트 사이클에 강제 전송
}

void APE_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 인벤토리는 보통 본인에게만 복제, 서버는 모든 state를 가지고있기때문
	DOREPLIFETIME_CONDITION(APE_PlayerState, InventoryData, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APE_PlayerState, CurrentQuantity, COND_OwnerOnly);
}

// 인벤토리 데이터가 변경될경우 자동으로 호출
void APE_PlayerState::OnRep_InventoryData()
{
	PRINT_LOG(TEXT("Is braodcated??"));
	OnInventoryChanged.Broadcast(InventoryData);
	Cast<APE_PlayerController>(GetPlayerController())->InventoryDataUpdate(InventoryData);
}

bool APE_PlayerState::IsEmptySlot(const int32 SlotNumber)
{
	if (InventoryData[SlotNumber].ItemID == NAME_None)
	{
		PRINT_LOG(TEXT("Slot is Empty"));
		return true;
	}

	return false;
}

void APE_PlayerState::AddItem(const FName ItemID, const int32 ItemQuantity)
{
	AddItem_Server(ItemID, ItemQuantity);
}

// 여기서 아이템 이미 있으면 추가, 없으면 가장 가까운 빈 슬롯에 추가
void APE_PlayerState::AddItem_Server_Implementation(const FName ItemID, const int32 ItemQuantity)
{
	int32 EmptySlotNum = -1;

	for (int32 i = 0; i < InventoryData.Num(); i++)
	{
		// 이미 같은 아이템을 가지고있는경우
		if (InventoryData[i].ItemID == ItemID)
		{
			InventoryData[i].Quantity += ItemQuantity;
			PRINT_LOG(TEXT("InventoryData[%d].ItemID : %s"), i, *InventoryData[i].ItemID.ToString());
			PRINT_LOG(TEXT("InventoryData[%d].Quantity : %d"), i, InventoryData[i].Quantity);
			OnInventoryChanged.Broadcast(InventoryData);
			if (GetPlayerController()->IsLocalController())
			{// 호스트의 클라이언트만 실행시키기 위함
				OnRep_InventoryData();
			}
			return;
		}

		// 첫번째 빈 슬롯
		if (EmptySlotNum < 0 && InventoryData[i].ItemID == NAME_None)
		{
			EmptySlotNum = i;
		}
	}

	// 같은 아이템을 가지고 있지 않은 경우
	if (EmptySlotNum >= 0)
	{
		InventoryData[EmptySlotNum].ItemID = ItemID;
		InventoryData[EmptySlotNum].Quantity = ItemQuantity;
		PRINT_LOG(TEXT("InventoryData[%d].ItemID : %s"), EmptySlotNum, *InventoryData[EmptySlotNum].ItemID.ToString());
		PRINT_LOG(TEXT("InventoryData[%d].Quantity : %d"), EmptySlotNum, InventoryData[EmptySlotNum].Quantity);
	}
	OnInventoryChanged.Broadcast(InventoryData);
	if (GetPlayerController()->IsLocalController())
	{
		OnRep_InventoryData();
	}
}

void APE_PlayerState::UseItem(const int32 SlotNumber)
{
	UseItem_Server(SlotNumber);
}

void APE_PlayerState::UseItem_Server_Implementation(const int32 SlotNumber)
{
	if (IsEmptySlot(SlotNumber))	return;

	InventoryData[SlotNumber].Quantity -= 1;

	// 개수가 0이 될경우 초기화
	if (InventoryData[SlotNumber].Quantity <= 0)
	{
		InventoryData[SlotNumber].ItemID = NAME_None;
		InventoryData[SlotNumber].Quantity = 0;
	}

	Cast<AProjectPlayer>(GetPlayerController()->GetCharacter())->GetInventoryComponent()->SpawnItem();

	OnInventoryChanged.Broadcast(InventoryData);
	if (GetPlayerController()->IsLocalController())
	{// 호스트의 클라이언트만 실행시키기 위함
		OnRep_InventoryData();
	}
}

void APE_PlayerState::OnRep_FuelData()
{
	OnFuelChanged.Broadcast(CurrentQuantity);
	PRINT_LOG(TEXT("Fuel BroadCast"));
	Cast<AProjectPlayer>(GetPlayerController()->GetCharacter())->UpdateFuel(CurrentQuantity);
}

bool APE_PlayerState::IsEnoughFuel(int32 Quantity)
{
	if (CurrentQuantity >= Quantity)	return true;

	PRINT_LOG(TEXT("Not Enough Fuel"));
	return false;
}

void APE_PlayerState::AddFuel(FName ItemID)
{
	AddFuel_Server(ItemID);
}

void APE_PlayerState::AddFuel_Server_Implementation(FName ItemID)
{
	SearchedItemData = nullptr;						// 이전결과 초기화

	for (FPE_ItemDataTable* Row : ItemDataRows)		// ItemDataTable에서 아이템 정보 검색
	{
		if (Row->ItemID == ItemID)
		{
			SearchedItemData = Row;
			break;
		}
	}

	if (!SearchedItemData) return;

	int32 Quantity = SearchedItemData->Quantity;

	if (CurrentQuantity + Quantity > MaxQuantity)
	{
		PRINT_LOG(TEXT("Fuel is Over"));
		return;
	}

	CurrentQuantity += Quantity;
	PRINT_LOG(TEXT("Fuel : %d"), CurrentQuantity);
	OnFuelChanged.Broadcast(CurrentQuantity);

	if (GetPlayerController()->IsLocalController())
	{
		OnRep_FuelData();
	}
}

void APE_PlayerState::UseFuel(int32 Quantity)
{
	UseFuel_Server(Quantity);
}

void APE_PlayerState::UseFuel_Server_Implementation(int32 Quantity)
{
	if (CurrentQuantity - Quantity >= 0)
	{
		CurrentQuantity -= Quantity;
	}
	else
	{
		PRINT_LOG(TEXT("Not enough Fuel"));
	}

	OnFuelChanged.Broadcast(CurrentQuantity);

	if (GetPlayerController()->IsLocalController())
	{
		OnRep_FuelData();
	}
}

