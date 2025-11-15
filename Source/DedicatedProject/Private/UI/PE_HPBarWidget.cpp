// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PE_HPBarWidget.h"
#include "Components/ProgressBar.h"
#include "DedicatedProject.h"
#include "HealthComponent.h"

void UPE_HPBarWidget::SetHealthPercent()
{
	if (!HPProgressBar)
	{
		PRINT_ERROR_LOG(TEXT("SetHealthPercent: HPProgressBar is NULL"));
		return;
	}

	if (!HealthComponent.IsValid())
	{
		PRINT_ERROR_LOG(TEXT("SetHealthPercent: HealthComponent is invalid"));
		HPProgressBar->SetPercent(0.0f);
		return;
	}

	if (nullptr != HPProgressBar)
	{
		float Ratio = HealthComponent->GetHPRatio();
		HPProgressBar->SetPercent(Ratio);
	}
}

void UPE_HPBarWidget::BindToHealthComponent(UHealthComponent* CurHealthComponent)
{
	if (HealthComponent == CurHealthComponent) return; // 이미 바인딩된 경우 중복 바인딩 방지

	if (HealthComponent.IsValid())
	{
		HealthComponent->OnHPChanged.RemoveAll(this); // 이전 바인딩 제거
	}

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
		if (HPProgressBar)
		{
			HPProgressBar->SetPercent(0.0f);
		}
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
		return;
	}

	HPProgressBar->SetPercent(1.0f); // 초기 체력 바를 100%로 설정
}
