// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTTask_WanderAroundTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"

#include "DedicatedProject.h"

UBTTask_WanderAroundTarget::UBTTask_WanderAroundTarget()
{

}

EBTNodeResult::Type UBTTask_WanderAroundTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AActor* Target = Cast<AActor>(BB->GetValueAsObject("Target"));
	if (!Target) return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(AIPawn);
	if (!NavSys) return EBTNodeResult::Failed;

	FVector TargetLocation = Target->GetActorLocation();
	FVector RandomPoint;
	bool bFoundLocation = NavSys->K2_GetRandomLocationInNavigableRadius(AIPawn, TargetLocation, RandomPoint, 300.0f);
	PRINT_LOG(TEXT("Wander To : %s"), *RandomPoint.ToString());
	if (!bFoundLocation) return EBTNodeResult::Failed;

	BB->SetValueAsVector("WanderPos", RandomPoint);
	BB->SetValueAsBool("bShouldWander", false);

	return EBTNodeResult::Succeeded;
}
