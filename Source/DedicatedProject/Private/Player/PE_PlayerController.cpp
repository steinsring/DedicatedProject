// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PE_PlayerController.h"
#include "DedicatedProject.h"
#include "UI/PE_Inventory.h"

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
}

void APE_PlayerController::InitializeDefaultData()
{

}

void APE_PlayerController::InitInventoryUI()
{
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
}

void APE_PlayerController::BeginPlay()
{
	InitInventoryUI();
}

