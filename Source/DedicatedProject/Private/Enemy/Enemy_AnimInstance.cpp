// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy_AnimInstance.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CharacterCommon.h"

#include "DedicatedProject.h"

// 몽타주 재생 시 애니메이션 노티파이로 호출되는 함수
void UEnemy_AnimInstance::AnimNotify_AttackRangeCheck()
{
	auto OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (!OwnerCharacter) return;

	auto AIController = Cast<AAIController>(OwnerCharacter->GetController());
	if (!AIController) return;

	auto BlackBoard = AIController->GetBlackboardComponent();
	if (!BlackBoard) return;

	auto Target = Cast<AActor>(BlackBoard->GetValueAsObject("Target"));
	if (!Target) return;

	bool bIsInAttackRange = (OwnerCharacter->GetDistanceTo(Target) <= 200.0f);
	PRINT_LOG(TEXT("AttackRangeCheck: %s"), bIsInAttackRange ? TEXT("In Range") : TEXT("Out of Range"));

	if (!bIsInAttackRange)
	{
		if (ACharacterCommon* Char = Cast<ACharacterCommon>(OwnerCharacter))
		{
			//BlackBoard->SetValueAsBool("bShouldWander", false);
			Char->StopMontage();
		}
	}
}

void UEnemy_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		AAIController* AIController = Cast<AAIController>(Pawn->GetController());
		if (AIController)
		{
			UBlackboardComponent* BlackBoard = AIController->GetBlackboardComponent();
			if (BlackBoard)
			{
				bool bWandering = BlackBoard->GetValueAsBool(TEXT("bShouldWander"));
				SetWandering(bWandering);
				//PRINT_LOG(TEXT("IsWandering: %s"), bWandering ? TEXT("True") : TEXT("False"));
			}
		}
	}
}

UAnimMontage* UEnemy_AnimInstance::GetRandomAttackMontage()
{
	if (AttackMontages.Num() == 0)
		return nullptr;
	int32 RandomIndex = FMath::RandRange(0, AttackMontages.Num() - 1);
	if (AttackMontages[RandomIndex] == nullptr)
	{
		PRINT_LOG(TEXT("AttackMontages[%d] is NULL"), RandomIndex);
		return nullptr;
	}
	//PRINT_LOG(TEXT("Montage Name: %s"), *AttackMontages[RandomIndex]->GetName());	
	
	return AttackMontages[RandomIndex];
}

void UEnemy_AnimInstance::PlayStunMontage(float Duration)
{
	IsStuned = true;
	StunDuration = Duration;

	if (!StunMontage)
	{
		PRINT_LOG(TEXT("StunMontage is NULL"));
		return;
	}
	Montage_Play(StunMontage, Duration);
}

void UEnemy_AnimInstance::EndStunMontage()
{
	IsStuned = false;
}
