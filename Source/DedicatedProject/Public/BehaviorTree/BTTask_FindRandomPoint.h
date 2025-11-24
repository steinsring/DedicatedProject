// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindRandomPoint.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UBTTask_FindRandomPoint : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FindRandomPoint();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
private:
	AAIController* AIController = nullptr;

	APawn* AIPawn = nullptr;

	AActor* Target = nullptr;

public:
	UPROPERTY(EditAnywhere, Category = "BlackBoard")
	float MinDistance = 200.0f;

	UPROPERTY(EditAnywhere, Category = "BlackBoard")
	float MaxDistance = 400.0f;
};
