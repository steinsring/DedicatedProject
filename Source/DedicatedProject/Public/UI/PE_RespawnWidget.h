// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/ProjectPlayer.h"
#include "PE_RespawnWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UPE_RespawnWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* RespawnListBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	TSubclassOf<class UPE_RespawnBtn> RespawnBtnClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	AProjectPlayer* TargetDummy;

	void InitializeRespawnList();

	UFUNCTION()
	void SetTargetDummy(AProjectPlayer* InTargetDummy);
};
