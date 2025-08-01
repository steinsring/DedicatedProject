// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PE_HPBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UPE_HPBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION()
	void SetHealthPercent();

	void BindToHealthComponent(class UHealthComponent* HealthComponent);

protected:
	virtual void NativeConstruct() override;
	
private:
	TWeakObjectPtr<class UHealthComponent> HealthComponent; // HealthComponent에 대한 약한 참조

	UPROPERTY()
	class UProgressBar* HPProgressBar;
};
