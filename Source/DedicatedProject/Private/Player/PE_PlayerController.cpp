// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PE_PlayerController.h"
#include "DedicatedProject.h"
#include "UI/PE_Inventory.h"
#include "Inventory/PE_InventoryComponent.h"
#include "Inventory/FItemData.h"
#include "UI/PE_HPBarWidget.h"
#include "HealthComponent.h"

APE_PlayerController::APE_PlayerController()
{
	// Inventory UI 로드 ----------------------------------------------------------------------
	static ConstructorHelpers::FClassFinder<UPE_Inventory> WidgetBPClass(TEXT("/Game/BluePrints/UI/WB_Inventory.WB_Inventory_C"));
	if (!WidgetBPClass.Succeeded())
	{
		PRINT_ERROR_LOG(TEXT("InventoryWidgetBPClass is NULL"));
		return;
	}

	InventoryWidgetBPClass = WidgetBPClass.Class;

	// HealthComp UI 로드 ----------------------------------------------------------------------
	static ConstructorHelpers::FClassFinder<UUserWidget> HPBarWidgetAsset(TEXT("WidgetBlueprint'/Game/BluePrints/UI/WB_HPBar.WB_HPBar_C'"));
	if (HPBarWidgetAsset.Succeeded())
	{
		HPBarWidgetClass = HPBarWidgetAsset.Class; //블루프린트에서 위젯 클래스를 불러온다.
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("HPBarWidgetAsset is NULL"));
	}

}

void APE_PlayerController::BeginPlay()
{
	if (!IsLocalController()) return;

	// Inventory UI 생성 ----------------------------------------------------------------------
	InventoryWidget = CreateWidget<UPE_Inventory>(this, InventoryWidgetBPClass);
	if (InventoryWidget)
	{
		InventoryWidget->AddToViewport();
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("InventoryWidget Create Fail"));
	}

	// 체력 --------------------------------------------------------------------------
	if (HPBarWidgetClass)
	{
		HPBarWidget = CreateWidget<UPE_HPBarWidget>(this, HPBarWidgetClass);
		if (HPBarWidget)
		{
			HPBarWidget->AddToViewport();
			HPBarWidget->BindToHealthComponent(GetPawn()->FindComponentByClass<UHealthComponent>());
		}
		else
		{
			PRINT_ERROR_LOG(TEXT("HPBarWidget is Not Created"));
		}
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("HPBarWidgetClass is NULL"));
	}
}

void APE_PlayerController::ItemSlotSelect(int32 i)
{ // 선택 위젯 슬롯 변경
	InventoryWidget->ItemSlotSelect(i);
}

void APE_PlayerController::InventoryDataUpdate(const TArray<FItemData>& InventoryData)
{
	InventoryWidget->SetInventoryData(InventoryData);
}

