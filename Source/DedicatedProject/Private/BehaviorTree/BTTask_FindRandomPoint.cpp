// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTTask_FindRandomPoint.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h" 

UBTTask_FindRandomPoint::UBTTask_FindRandomPoint()
{
	NodeName = (TEXT("MoveToRandomPoint"));
}

EBTNodeResult::Type UBTTask_FindRandomPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	AIPawn = AIController->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	Target = Cast<AActor>(BB->GetValueAsObject("Target"));
	if (!Target) return EBTNodeResult::Failed;

	FVector TargetLocation = Target->GetActorLocation();

	// 네비게이션 시스템 가져오기
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return EBTNodeResult::Failed;

	FNavLocation OutLocation;

	// 일단 MaxDistance 안에서 랜덤 위치 하나 뽑기
	const int32 MaxTryCount = 8;
	bool bFound = false;

	for (int32 i = 0; i < MaxTryCount; ++i)
	{
		if (NavSys->GetRandomPointInNavigableRadius(TargetLocation, MaxDistance, OutLocation))
		{
			// 타겟과의 실제 거리 체크 (Min ~ Max 사이인지)
			const float Dist = FVector::Dist2D(TargetLocation, OutLocation.Location);
			if (Dist >= MinDistance && Dist <= MaxDistance)
			{
				bFound = true;
				break;
			}
		}
	}

	if (!bFound)
	{
		return EBTNodeResult::Failed;
	}

	BB->SetValueAsVector(TEXT("WanderPos"), OutLocation.Location);

	return EBTNodeResult::Succeeded;
}
