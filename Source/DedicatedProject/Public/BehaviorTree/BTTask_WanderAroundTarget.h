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

	void ReturnSpeedToOriginal();
	void StartOrbitMovement();

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	// 조정 가능한 파라미터들
	UPROPERTY(EditAnywhere, Category = "Wander Settings")
	float CloseDistance = 400.0f; // 가까움 기준

	UPROPERTY(EditAnywhere, Category = "Wander Settings")
	float RetreatDistance = 300.0f; // 후퇴 거리

	UPROPERTY(EditAnywhere, Category = "Wander Settings")
	float OrbitAngleDegree = 120.0f; // 이동할 호의 각도

	UPROPERTY(EditAnywhere, Category = "Wander Settings")
	float MinWanderTime = 2.0f;

private:
	bool bIsWandering = false;

	AAIController* AIController = nullptr;

	APawn* AIPawn = nullptr;

	AActor* Target = nullptr;

	FVector TargetPos;

	float OriginalSpeed = 0.0f;

	UEnemy_AnimInstance* AnimInstance = nullptr;

	float ElapsedWanderTime = 0.0f;

	bool bHasStartedOrbit = false;

	bool bHasRetreated = false;
};
