// Fill out your copyright notice in the Description page of Project Settings.


#include "Fuel/PE_FuelComponent.h"
#include "Fuel/PE_FuelWidget.h"
#include "DedicatedProject.h"

UPE_FuelComponent::UPE_FuelComponent()
{
	static ConstructorHelpers::FClassFinder<UPE_FuelWidget> WidgetBPClass(TEXT("/Game/BluePrints/UI/WB_FuelWidget.WB_FuelWidget_C"));
	if (WidgetBPClass.Succeeded())
	{
		FuelWidget = WidgetBPClass.Class;
	}
	else
	{
		PRINT_LOG(TEXT("WidgetBPClass is NULL"));
	}

	SetWidgetSpace(EWidgetSpace::World);
	SetDrawAtDesiredSize(false);					//위젯이 월드에서 차지할 크기를 직접 지정
	SetDrawSize(FVector2D(500.f, 500.f));
	SetWidgetClass(FuelWidget);
	SetRelativeLocation(FVector(-4.f, -30.f, 0.f));	// 배낭 옆 약간 띄우기
	SetRelativeRotation(FRotator(0.f, 0.f, -50.f));
	SetTranslucentSortPriority(5);					// 반투명 오브젝트의 렌더링 우선순위를 지정
	SetTwoSided(true);								// 위젯이 앞뒤 양쪽
	SetWorldScale3D(FVector(1.f, 0.03f, 0.1f));		// 실제 scale
}

void UPE_FuelComponent::BeginPlay()
{
	Super::BeginPlay();

	// 컴포넌트가 내부적으로 만든 인스턴스
	FuelWidgetInstance = Cast<UPE_FuelWidget>(GetWidget());
}

void UPE_FuelComponent::AddFuel(FName ItemID)
{
	AddFuel_Server(ItemID);
}

void UPE_FuelComponent::AddFuel_Server_Implementation(FName ItemID)
{
	FuelWidgetInstance->AddFuel(ItemID);
}

void UPE_FuelComponent::UseFuel(int32 Quantity)
{
	UseFuel_Server(Quantity);
}

void UPE_FuelComponent::UseFuel_Server_Implementation(int32 Quantity)
{
	FuelWidgetInstance->UseFuel(Quantity);
}

bool UPE_FuelComponent::HasEnoughFuel(int32 Quantity) const
{
	if (FuelWidgetInstance->GetCurrentFuel() >= Quantity)
	{
		return true;
	}
	return false;
}