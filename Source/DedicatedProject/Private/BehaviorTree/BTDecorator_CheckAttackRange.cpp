// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTDecorator_CheckAttackRange.h"
#include "Player/ProjectPlayer.h"
#include "Enemy/PE_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CheckAttackRange::UBTDecorator_CheckAttackRange()
{
	NodeName = TEXT("CanAttack");
}

bool UBTDecorator_CheckAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return false;

	auto Target = Cast<AProjectPlayer>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(APE_AIController::TargetKey));
	if (nullptr == Target) return false;

	bResult = (Target->GetDistanceTo(ControllingPawn) <= 200.0f);
	return bResult;
}
