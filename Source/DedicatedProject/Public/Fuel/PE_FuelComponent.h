// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "PE_FuelComponent.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UPE_FuelComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, Category = "FuelWidget")
	TSubclassOf<class UPE_FuelWidget> FuelWidget;	// FuelComponent

	UPROPERTY(Transient) // 저장/로드/에디터 저장 등에 포함되지 않는, 런타임 전용 값
	TObjectPtr<UPE_FuelWidget> FuelWidgetInstance;

	UFUNCTION(Server, Reliable)
	void AddFuel_Server(FName ItemID);


protected:
	virtual void BeginPlay() override;

public:
	UPE_FuelComponent();

	void AddFuel(FName ItemID);
};
