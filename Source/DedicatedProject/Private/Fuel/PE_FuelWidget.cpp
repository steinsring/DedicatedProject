// Fill out your copyright notice in the Description page of Project Settings.


#include "Fuel/PE_FuelWidget.h"
#include "DedicatedProject.h"

void UPE_FuelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	FuelProgressBar->SetPercent(0.f);
}

void UPE_FuelWidget::UpdateFuelUI(const int32 Quantity)
{
	CurrentQuantity = Quantity;
	
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


