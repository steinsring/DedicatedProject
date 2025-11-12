// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/ProjectPlayer.h"
#include "UI/PE_RespawnWidget.h"
#include "PE_RespawnBtn.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UPE_RespawnBtn : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void OnClick_RespawnBtn(APlayerState* DeadPlayerState, AProjectPlayer* TargetDummy);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	APlayerState* RespawnPlayerState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	AProjectPlayer* RespawnTargetDummy;

	UPROPERTY()
	class UPE_RespawnWidget* ParentRespawnWidget;
};
