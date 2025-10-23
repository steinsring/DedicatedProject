// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WanderAroundTarget.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UBTTask_WanderAroundTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_WanderAroundTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
