// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PE_InventorySlot.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class DEDICATEDPROJECT_API UPE_InventorySlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UImage* IconImage;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* Number;

	void SetIcon(UTexture2D* Icon);
};
