// Fill out your copyright notice in the Description page of Project Settings.


#include "Fuel/PE_FuelWidget.h"
#include "DedicatedProject.h"

void UPE_FuelWidget::NativeConstruct()
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
	FuelProgressBar->SetPercent(0.f);
}

void UPE_FuelWidget::FindItemData(FName ItemID)
{
	SearchedItemData = nullptr;						// 이전결과 초기화

	for (FPE_ItemDataTable* Row : ItemDataRows)		// ItemDataTable에서 아이템 정보 검색
	{
		if (Row->ItemID == ItemID)
		{
			SearchedItemData = Row;
			//PRINT_LOG(TEXT("Find Item Success"));
			return;
		}
	}
	PRINT_LOG(TEXT("Find Item Fail"));
}

void UPE_FuelWidget::AddFuel(FName ItemID)
{
	FindItemData(ItemID);
	int32 Quantity = SearchedItemData->Quantity;

	if(CurrentQuantity + Quantity < MaxQuantity)
	{
		CurrentQuantity += Quantity;
	}
	else
	{

	}
	float Percent = (float)CurrentQuantity / MaxQuantity;
	FuelProgressBar->SetPercent(Percent);
	PRINT_LOG(TEXT("Add Fuel : %d, Max : %d, percent : %f"), CurrentQuantity, MaxQuantity, Percent);
}

void UPE_FuelWidget::UseFuel(int32 Quantity)
{
	if (CurrentQuantity - Quantity >= 0)
	{
		CurrentQuantity -= Quantity;

		float Percent = (float)CurrentQuantity / MaxQuantity;
		FuelProgressBar->SetPercent(Percent);
		PRINT_LOG(TEXT("Use Fuel : %d"), Quantity);
		PRINT_LOG(TEXT("Remain Fuel : %d, Max : %d, percent : %f"), CurrentQuantity, MaxQuantity, Percent);
	}
	else
	{
		PRINT_LOG(TEXT("Not enough Fuel"));
	}
}


