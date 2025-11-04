// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTTask_TurnOffRotationMovement.h"
#include "behaviortree/BlackboardComponent.h"

#include "DedicatedProject.h"


UBTTask_TurnOffRotationMovement::UBTTask_TurnOffRotationMovement()
{
	NodeName = (TEXT("TurnOffRotation"));
}

EBTNodeResult::Type UBTTask_TurnOffRotationMovement::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if (!BlackBoard)
	{
		return EBTNodeResult::Failed;
	}

	BlackBoard->SetValueAsBool(TEXT("bShouldWander"), false);
	return EBTNodeResult::Succeeded;
}
