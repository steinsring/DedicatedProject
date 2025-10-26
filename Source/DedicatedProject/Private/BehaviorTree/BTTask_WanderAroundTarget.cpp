// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTTask_WanderAroundTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"

#include "DedicatedProject.h"

UBTTask_WanderAroundTarget::UBTTask_WanderAroundTarget()
{
	NodeName = (TEXT("WanderAroundTarget"));
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_WanderAroundTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	AIPawn = AIController->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	Target = Cast<AActor>(BB->GetValueAsObject("Target"));
	if (!Target) return EBTNodeResult::Failed;

	ACharacter* AIChar = Cast<ACharacter>(AIPawn);
	if (!AIChar) return EBTNodeResult::Failed;

	UCharacterMovementComponent* MoveComp = AIChar->GetCharacterMovement();
	if (MoveComp)
	{
		OriginalSpeed = MoveComp->MaxWalkSpeed;
		MoveComp->MaxWalkSpeed = OriginalSpeed * 0.1f;
	}

	FVector Center = Target->GetActorLocation();
	FVector ToAI = AIPawn->GetActorLocation() - Center;
	ToAI.Z = 0.0f;
	ToAI.Normalize();

	FVector TangentDir = FMath::RandBool() ? FVector(-ToAI.Y, ToAI.X, 0.0f) : FVector(ToAI.Y, -ToAI.X, 0.0f);

	float MoveDistance = OrbitRadius * FMath::DegreesToRadians(OrbitAngleDegree);

	TargetPos = AIPawn->GetActorLocation() + TangentDir * MoveDistance;

	FVector AIPos = AIPawn->GetActorLocation();

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(AIPawn);
	if (!NavSys) return EBTNodeResult::Failed;

	FNavLocation NavLocation;
	if (NavSys->ProjectPointToNavigation(TargetPos, NavLocation))
	{
		AIController->MoveToLocation(NavLocation.Location, -1.0f, true, true, false, true, 0, true);

		return EBTNodeResult::InProgress;
	}
	
	return EBTNodeResult::Failed;
}

void UBTTask_WanderAroundTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!AIController || !AIPawn || !Target)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FVector ToTarget = Target->GetActorLocation() - AIPawn->GetActorLocation();
	ToTarget.Z = 0.0f;

	FRotator LookRot = FRotationMatrix::MakeFromX(ToTarget).Rotator();
	AIPawn->SetActorRotation(LookRot);

	if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		if (UCharacterMovementComponent* MoveComp = Cast<ACharacter>(AIPawn)->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = OriginalSpeed;
		}

		UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
		BB->SetValueAsBool(TEXT("bShouldWander"), false);

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
