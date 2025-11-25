// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WatchTarget.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UBTTask_WatchTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
	
public:
	UBTTask_WatchTarget();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
