// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTTask_WanderAroundTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"

#include "CharacterCommon.h"

#include "DedicatedProject.h"

UBTTask_WanderAroundTarget::UBTTask_WanderAroundTarget()
{
	NodeName = (TEXT("WanderAroundTarget"));
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_WanderAroundTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ElapsedWanderTime = 0.0f;

	AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	AIPawn = AIController->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	Target = Cast<AActor>(BB->GetValueAsObject("Target"));
	if (!Target) return EBTNodeResult::Failed;

	ACharacter* AIChar = Cast<ACharacter>(AIPawn);
	if (!AIChar) return EBTNodeResult::Failed;

	ACharacterCommon* Character = Cast<ACharacterCommon>(AIChar);
	if (!Character) return EBTNodeResult::Failed;

	//속도 설정
	UCharacterMovementComponent* MoveComp = AIChar->GetCharacterMovement();
	if (MoveComp)
	{
		PRINT_LOG(TEXT("MaxSpeed : %f"), MoveComp->MaxWalkSpeed);
		OriginalSpeed = Character->GetSpeed();
		MoveComp->MaxWalkSpeed = OriginalSpeed;
	}

	AnimInstance = Cast<UEnemy_AnimInstance>(AIChar->GetMesh()->GetAnimInstance());
	if (AnimInstance) AnimInstance->SetGoingLeft(FMath::RandBool());

	//중심과 거리 계산
	FVector Center = Target->GetActorLocation();
	FVector AIPos = AIPawn->GetActorLocation();
	FVector ToAI = AIPos - Center;
	ToAI.Z = 0.0f;
	float Distance = ToAI.Size();
	ToAI.Normalize();

	//너무 가까우면 후퇴
	if (Distance < CloseDistance)
	{
		FVector RetreatPos = AIPos + (-ToAI * RetreatDistance);

		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(AIPawn);
		if (!NavSys) return EBTNodeResult::Failed;

		FNavLocation NavLocation;
		if (NavSys->ProjectPointToNavigation(TargetPos, NavLocation))
		{
			AIController->MoveToLocation(NavLocation.Location, -1.0f, true, true, false, true, 0, true);
			bHasRetreated = true;
			return EBTNodeResult::InProgress;
		}
	}
	
	//호 이동 시작
	StartOrbitMovement();
	return EBTNodeResult::InProgress;
}

void UBTTask_WanderAroundTarget::ReturnSpeedToOriginal()
{
	ACharacterCommon* AIChar = Cast<ACharacterCommon>(AIPawn);
	if (!AIChar) return;

	UCharacterMovementComponent* MoveComp = AIChar->GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->MaxWalkSpeed = AIChar->GetMaxSpeed();
	}
}

void UBTTask_WanderAroundTarget::StartOrbitMovement()
{
	FVector Center = Target->GetActorLocation();
	FVector AIPos = AIPawn->GetActorLocation();
	FVector ToAI = AIPos - Center;
	ToAI.Z = 0.0f;

	float CurrentRadius = ToAI.Size();
	ToAI.Normalize();
	
	//좌, 우 결정
	float AngleDeltaDegree = OrbitAngleDegree;
	if (!AnimInstance->IsGoingLeft)
	{
		AngleDeltaDegree *= -1.0f;
	}

	//현재 벡터를 중심 기준으로 회전시킴
	FVector RotatedVector = ToAI.RotateAngleAxis(AngleDeltaDegree, FVector::UpVector);
	//목표 위치 계산
	TargetPos = Center + RotatedVector * CurrentRadius;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(AIPawn);
	if (!NavSys) return;
	FNavLocation NavLocation;
	if (NavSys->ProjectPointToNavigation(TargetPos, NavLocation))
	{
		AIController->MoveToLocation(NavLocation.Location, -1.0f, true, true, false, true, 0, true);
		bHasStartedOrbit = true;
	}
}



void UBTTask_WanderAroundTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!AIController || !AIPawn || !Target)
	{
		ReturnSpeedToOriginal();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FVector ToTarget = Target->GetActorLocation() - AIPawn->GetActorLocation();
	ToTarget.Z = 0.0f;

	//적을 바라보도록 회전
	FRotator LookRot = FRotationMatrix::MakeFromX(ToTarget).Rotator();
	AIPawn->SetActorRotation(LookRot);

	ElapsedWanderTime += DeltaSeconds;

	if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		//후퇴 후에 도착한 경우, 호 이동 시작
		if (bHasRetreated && !bHasStartedOrbit)
		{
			bHasRetreated = false;
			StartOrbitMovement();
			return;
		}

		//호 이동이 끝난 경우, 태스크 종료
		if (bHasStartedOrbit)
		{
			UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
			BB->SetValueAsBool(TEXT("bShouldWander"), false);

			ReturnSpeedToOriginal();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}
