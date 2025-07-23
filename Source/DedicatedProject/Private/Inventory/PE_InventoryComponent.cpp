// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/PE_InventoryComponent.h"
#include "Inventory/PE_InventoryItemBase.h"
#include "Blueprint/UserWidget.h"
#include "UI/PE_Inventory.h"
#include "DedicatedProject.h"

// Sets default values for this component's properties
UPE_InventoryComponent::UPE_InventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/DataTable/DT_ItemDataTable.DT_ItemDataTable"));

	if (ItemDataTable)									//데이터 테이블이 참조되었는지 확인
	{
		ItemDataTable->GetAllRows<FPE_ItemDataTable>(TEXT("ProjectPlayer"), ItemDataRows); //테이블의 모든 행을 지역배열로 가져온다.
	}
	else
	{
		PRINT_LOG(TEXT("ItemDataTable is NULL"));
	}
	
	static ConstructorHelpers::FClassFinder<UPE_Inventory> WidgetBPClass(TEXT("/Game/BluePrints/UI/WB_Inventory.WB_Inventory_C"));
	if (WidgetBPClass.Succeeded())
	{
		InventoryWidgetClass = WidgetBPClass.Class;
	}
	else
	{
		PRINT_LOG(TEXT("WidgetBPClass is NULL"));
	}
}


// Called when the game starts
void UPE_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!InventoryWidgetClass) 
	{
		return;
	}

	// 소유자에서 컨트롤러 가져오기
	APlayerController* PlayerContorller = GetWorld()->GetFirstPlayerController();
	if (!PlayerContorller) 
	{
		return;
	}
	PRINT_LOG(TEXT("Is Begin twice?"));
	// 위젯 생성
	InventoryWidget = CreateWidget<UPE_Inventory>(PlayerContorller, InventoryWidgetClass);
	if (InventoryWidget)
	{
		InventoryWidget->AddToViewport();
	}
}


// Called every frame
void UPE_InventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPE_InventoryComponent::FindItemData(FName ItemID)
{
	SearchedItemData = nullptr;						// 이전결과 초기화

	for (FPE_ItemDataTable* Row : ItemDataRows)		// ItemDataTable에서 아이템 정보 검색
	{
		if (Row->ItemID == ItemID)
		{
			SearchedItemData = Row;
			PRINT_LOG(TEXT("Find Item Success"));
			return;
		}
	}
	PRINT_LOG(TEXT("Find Item Fail"));
}


bool UPE_InventoryComponent::AddItem(FName ItemID, int32 Quantity)
{
	FindItemData(ItemID);
	InventoryWidget->AddItemToInventory(SearchedItemData->Icon);
	return true;
}

bool UPE_InventoryComponent::RemoveItem(UPE_InventoryItemBase* Item, int32 Quantity)
{
	return true;
}
