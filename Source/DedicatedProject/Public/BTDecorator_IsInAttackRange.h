// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DedicatedProject.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsInAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UBTDecorator_IsInAttackRange : public UBTDecorator
{
	GENERATED_BODY()
	
	
public:
	UBTDecorator_IsInAttackRange();
	
private:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
