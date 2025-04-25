// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPatrolPos.h"
#include "PE_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

//이 클래스를 통해 BehaviorTree에 FindPatrolPos라는 태스크(노드)가 만들어짐
UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	NodeName = TEXT("FindPatrolPos");
}

//태스크는 Update처럼 ExecuteTask함수가 계속 실행되며 그때마다 Failed, Inprogress, Succeeded를 반환한다.
EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

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
	if (NavSystem->GetRandomPointInNavigableRadius(Origin, 500.0f, NextPatrol))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(APE_AIController::PatrolPosKey, NextPatrol.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}