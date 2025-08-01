// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PE_HPBarWidget.h"
#include "Components/ProgressBar.h"
#include "DedicatedProject.h"
#include "HealthComponent.h"

void UPE_HPBarWidget::SetHealthPercent()
{
	if (nullptr != HPProgressBar)
	{
		HPProgressBar->SetPercent(HealthComponent->GetHPRatio());
	}
}

void UPE_HPBarWidget::BindToHealthComponent(UHealthComponent* CurHealthComponent)
{
	HealthComponent = CurHealthComponent; // Store the reference to the HealthComponent

	if (CurHealthComponent)
	{
		// Bind the SetHealthPercent function to the OnHPChanged delegate of the HealthComponent
		CurHealthComponent->OnHPChanged.AddUObject(this, &UPE_HPBarWidget::SetHealthPercent);

		// Optionally, set the initial health percent
		SetHealthPercent();

		// Bind to OnHPIsZero if you want to handle death or zero health
		// HealthComponent->OnHPIsZero.AddDynamic(this, &UPE_HPBarWidget::HandleZeroHealth);
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("HealthComponent is NULL in UPE_HPBarWidget::BindToHealthComponent"));
	}
}

void UPE_HPBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// Find the ProgressBar in the widget hierarchy
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthBar")));
	if (!HPProgressBar)
	{
		PRINT_ERROR_LOG(TEXT("HealthBar not found in PE_HPBarWidget"));
	}
}
