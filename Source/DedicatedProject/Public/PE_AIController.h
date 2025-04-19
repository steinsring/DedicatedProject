// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "PE_AIController.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API APE_AIController : public AAIController
{
	GENERATED_BODY()
	
public:
	APE_AIController();
	virtual void OnPossess(APawn* InPawn) override;
	
private:
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	class UBlackboardData* BBAsset;
};
