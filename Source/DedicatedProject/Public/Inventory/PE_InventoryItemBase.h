// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PE_InventoryItemBase.generated.h"

class AProjectPlayer;

UCLASS()
class DEDICATEDPROJECT_API UPE_InventoryItemBase : public UObject
{
	GENERATED_BODY()
	
private:
    UPROPERTY()
    FText ItemName;

    UPROPERTY()
    TObjectPtr<class UTexture2D> Icon;

    UPROPERTY()
    int32 MaxStackSize = 1;
public:
    // 추가 기능: 사용, 장착, 버리기 등
    //UFUNCTION()
    //virtual void Use(class AProjectPlayer* Character);
	
    UFUNCTION(BlueprintCallable)
    void Init(FText NewItemName, UTexture2D* NewIcon, int32 NewMaxStackSize = 1)
    {
        ItemName = NewItemName;
        Icon = NewIcon;
        MaxStackSize = NewMaxStackSize;
    }
};
