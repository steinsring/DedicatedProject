// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
//#include "BehaviorTree/BehaviorTree.h"
//#include "BehaviorTree/BlackboardData.h"
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

	//Blackboard에 선언된 vector 키들의 string값
	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;
	
private:
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	class UBlackboardData* BBAsset;

	UPROPERTY()
	class UBlackboardComponent* BBComp;
};
