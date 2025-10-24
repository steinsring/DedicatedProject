// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PE_InventorySlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "DedicatedProject.h"
#include "Inventory/FItemData.h"


void UPE_InventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

	ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/DataTable/DT_ItemDataTable.DT_ItemDataTable"));

	if (ItemDataTable)									//데이터 테이블이 참조되었는지 확인
	{
		ItemDataTable->GetAllRows<FPE_ItemDataTable>(TEXT("ProjectPlayer"), ItemDataRows); //테이블의 모든 행을 지역배열로 가져온다.
	}
	else
	{
		PRINT_LOG(TEXT("ItemDataTable is NULL"));
	}
}

void UPE_InventorySlot::FindItemData(FName ItemID)
{
	SearchedItemData = nullptr;						// 이전결과 초기화

	for (FPE_ItemDataTable* Row : ItemDataRows)		// ItemDataTable에서 아이템 정보 검색
	{
		if (Row->ItemID == ItemID)
		{
			SearchedItemData = Row;
			return;
		}
	}
	PRINT_LOG(TEXT("Find Item Fail"));
}

void UPE_InventorySlot::SetItem(FName ItemID, int32 ItemQuantity)
{
	// 아이템을 다쓴 경우
	if (ItemID == NAME_None)
	{
		ID = NAME_None;
		Stack = 0;
		IconImage->SetBrushFromTexture(nullptr);
		IconImage->SetVisibility(ESlateVisibility::Hidden);
		Number->SetText(FText::FromString(TEXT(" ")));
		return;
	}

	// 아이템이 남은 경우
	FindItemData(ItemID);
	if (IsValid(IconImage))
	{
		ID = ItemID;
		Stack = ItemQuantity;
		IconImage->SetBrushFromTexture(SearchedItemData->Icon, true);
		IconImage->SetVisibility(ESlateVisibility::Visible);
		Number->SetText(FText::AsNumber(Stack));
	}
	else
	{
		PRINT_LOG(TEXT("IconImage is NULL"));
	}
}

FName UPE_InventorySlot::GetSlotInformation()
{
	return ID;
}

void UPE_InventorySlot::UseItem()
{
	// 아이템이 1개 남은경우 빈슬롯 만들기
	if (Stack <= 1)
	{
		Stack = 0;
		IconImage->SetBrushFromTexture(nullptr);
		IconImage->SetVisibility(ESlateVisibility::Hidden);
		Number->SetText(FText::FromString(TEXT(" ")));
		ID = NAME_None;
		return;
	}

	// 그렇지 않으면 stack 1 감소
	Stack--;
	Number->SetText(FText::AsNumber(Stack));
}

