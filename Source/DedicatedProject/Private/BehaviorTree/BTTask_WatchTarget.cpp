// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTTask_WatchTarget.h"
#include "Player/ProjectPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/PE_AIController.h"

UBTTask_WatchTarget::UBTTask_WatchTarget()
{
	NodeName = (TEXT("WatchTarget"));
}

EBTNodeResult::Type UBTTask_WatchTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
	AProjectPlayer* Target = Cast<AProjectPlayer>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("PrevTarget")));
	
	if (!Pawn || !Target) return EBTNodeResult::Failed;

	FVector Direction = Target->GetActorLocation() - Pawn->GetActorLocation();
	Direction.Z = 0.0f;

	FRotator TargetRot = Direction.Rotation();
	AIController->SetControlRotation(TargetRot);
	Pawn->SetActorRotation(TargetRot);
	
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("bShouldWander"), false);
	return EBTNodeResult::Succeeded;
}
