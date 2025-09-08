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
	
public:
	APE_PlayerController();
	
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	USkillManagerComponent* SkillManager;*/
};
