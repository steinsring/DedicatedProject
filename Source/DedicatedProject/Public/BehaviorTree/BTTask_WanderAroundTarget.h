// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Enemy/Enemy_AnimInstance.h"
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

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wander")
	float OrbitAngleDegree = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wander")
	float OrbitRadius = 300.0f;

private:
	bool bIsWandering = false;

	AAIController* AIController = nullptr;

	APawn* AIPawn = nullptr;

	AActor* Target = nullptr;

	FVector TargetPos;

	float OriginalSpeed = 0.0f;

	UEnemy_AnimInstance* AnimInstance = nullptr;

	float MinWanderTime = 2.0f;
	float ElapsedWanderTime = 0.0f;
};
