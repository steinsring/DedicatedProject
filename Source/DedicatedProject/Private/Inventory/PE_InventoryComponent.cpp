// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/PE_InventoryComponent.h"
#include "Inventory/PE_InventoryItemBase.h"

// Sets default values for this component's properties
UPE_InventoryComponent::UPE_InventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPE_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPE_InventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


bool UPE_InventoryComponent::AddItem(UPE_InventoryItemBase* Item, int32 Quantity)
{
	return true;
}

bool UPE_InventoryComponent::RemoveItem(UPE_InventoryItemBase* Item, int32 Quantity)
{
	return true;
}
