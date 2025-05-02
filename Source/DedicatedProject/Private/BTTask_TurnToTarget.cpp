// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "PE_AIController.h"
#include "ProjectPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = (TEXT("Turn"));
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ProjectPlayer = Cast<AProjectPlayer>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ProjectPlayer)
		return EBTNodeResult::Failed;

	auto Target = Cast<AProjectPlayer>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(APE_AIController::TargetKey));
	if (nullptr == Target)
		return EBTNodeResult::Failed;

	FVector LookVector = Target->GetActorLocation() - ProjectPlayer->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	ProjectPlayer->SetActorRotation(FMath::RInterpTo(ProjectPlayer->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.0f));

	return EBTNodeResult::Succeeded;
}
