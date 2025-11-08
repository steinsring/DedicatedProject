// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SkillManagerComponent.h"
#include "PE_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API APE_PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	APE_PlayerController();

private:
	// 인벤토리 ----------------------------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TSubclassOf<class UPE_Inventory> InventoryWidgetBPClass;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<class UPE_Inventory> InventoryWidget;				// 실제 생성된 인벤토리 위젯 인스턴스

public:
	void ItemSlotSelect(int32 i);

	void InventoryDataUpdate(const TArray<struct FItemData>& InventoryData);

	UFUNCTION()
	void OnNextSpectateTarget();

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Spectator;

private:
	// 체력 ------------------------------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TSubclassOf<class UPE_HPBarWidget> HPBarWidgetClass; // 플레이어 UI 위젯 컴포넌트

	UPROPERTY()
	TObjectPtr<UPE_HPBarWidget> HPBarWidget; // 플레이어 UI 위젯 인스턴스
};
