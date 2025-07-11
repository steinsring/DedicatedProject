// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTService_CheckAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/PE_AIController.h"
#include "Player/ProjectPlayer.h"

UBTService_CheckAttackRange::UBTService_CheckAttackRange()
{
	NodeName = TEXT("Check Attack Range");
	Interval = 0.2f;
}

void UBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	auto Target = Cast<AProjectPlayer>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(APE_AIController::TargetKey));
	if (nullptr == Target) return;

	auto BlackBoard = OwnerComp.GetBlackboardComponent();

	const float Distance = FVector::Dist(ControllingPawn->GetActorLocation(), Target->GetActorLocation());
	const bool bInRage = Distance <= 200.0f;

	BlackBoard->SetValueAsBool(TEXT("IsInAttackRange"), bInRage);
}
