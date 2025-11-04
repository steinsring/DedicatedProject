// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTTask_FindPatrolPos.h"
#include "Enemy/PE_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

//이 클래스를 통해 BehaviorTree에 FindPatrolPos라는 태스크(노드)가 만들어짐
UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	NodeName = TEXT("FindPatrolPos");
}

//태스크는 해당 TaskNode가 실행될 때 한번만 호출되는 함수이며, Failed, Inprogress, Succeeded를 반환한다.
EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("bShouldWander"), false);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
		return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if(nullptr == NavSystem)
		return EBTNodeResult::Failed;

	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(APE_AIController::HomePosKey);
	FNavLocation NextPatrol;

	//origin으로부터 반경 500에 랜덤한 포인트가 NextPatrol이 된다.
	//500.0f는 ai가 움직일 반경의 크기(에디터에서 NavMeshBoundsVolume의 크기를 생각해서 세팅해줘야해)
	//if (NavSystem->GetRandomPointInNavigableRadius(Origin, 500.0f, NextPatrol))
	//{
	//	OwnerComp.GetBlackboardComponent()->SetValueAsVector(APE_AIController::PatrolPosKey, NextPatrol.Location);
	//	return EBTNodeResult::Succeeded;
	//}

	APE_AIController* Controller = Cast<APE_AIController>(OwnerComp.GetAIOwner());
	if (nullptr == Controller)
		return EBTNodeResult::Failed;
	
	PatrolPoints = Controller->WayPoints;
	if (PatrolPoints.Num() == 0) 
		return EBTNodeResult::Failed;
	UE_LOG(LogTemp, Log, TEXT("PatrolPoints Count : %d"), PatrolPoints.Num());

	CurrentPatrolIndex = FMath::Clamp(CurrentPatrolIndex, 0, PatrolPoints.Num() - 1);
	if (!PatrolPoints.IsValidIndex(CurrentPatrolIndex))
		return EBTNodeResult::Failed;

	float Distance = FVector::Dist(ControllingPawn->GetActorLocation(), PatrolPoints[CurrentPatrolIndex]->GetActorLocation());
	//UE_LOG(LogTemp, Log, TEXT("Dist : %f, CurrentPatrolIndex : %d"), Distance, CurrentPatrolIndex);

	if (Distance < 100.0f)
	{
		CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
	}

	if (!PatrolPoints.IsValidIndex(CurrentPatrolIndex))
		return EBTNodeResult::Failed;

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(APE_AIController::PatrolPosKey, PatrolPoints[CurrentPatrolIndex]->GetActorLocation());
	//UE_LOG(LogTemp, Log, TEXT("PatrolPos : %s"), *OwnerComp.GetBlackboardComponent()->GetValueAsVector(APE_AIController::PatrolPosKey).ToString());
	return EBTNodeResult::Succeeded;

	//return EBTNodeResult::Failed;
}