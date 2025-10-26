// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTTask_TurnToTarget.h"
#include "Enemy/PE_AIController.h"
#include "Player/ProjectPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = (TEXT("Turn"));

    bNotifyTick = true;
    bCreateNodeInstance = true;

    AcceptableAngle = 5.0f;     // 회전 완료로 간주할 각도
    RotationSpeed = 5.0f;       // 회전 속도 (보간에 사용)
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // TickTask에서 처리하니까 여기선 InProgress
    return EBTNodeResult::InProgress;
}

void UBTTask_TurnToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
    AProjectPlayer* Target = Cast<AProjectPlayer>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(APE_AIController::TargetKey));

    if (!Pawn || !Target)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    FVector Direction = Target->GetActorLocation() - Pawn->GetActorLocation();
    Direction.Z = 0.0f;

    FRotator TargetRot = Direction.Rotation();
    FRotator CurrentRot = Pawn->GetActorRotation();

    FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaSeconds, RotationSpeed);
	AIController->SetControlRotation(NewRot);
    Pawn->SetActorRotation(NewRot);

    float AngleDiff = FMath::Abs(FRotator::NormalizeAxis((TargetRot - NewRot).Yaw));
    if (AngleDiff < AcceptableAngle)
    {
		ACharacter* Character = Cast<ACharacter>(Pawn);
        if (Character)
        {
            Character->bUseControllerRotationYaw = false;
			if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
			{
				MoveComp->bOrientRotationToMovement = true;
			}
        }

        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
