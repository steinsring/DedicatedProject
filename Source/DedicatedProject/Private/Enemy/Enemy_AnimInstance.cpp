// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy_AnimInstance.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"

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

	const bool bIsInAttackRange = BlackBoard->GetValueAsBool("IsInAttackRange");

	if (!bIsInAttackRange)
	{
		UAnimMontage* CurrentMontage = GetCurrentActiveMontage();
		if (CurrentMontage)
		{
			Montage_Stop(0.2f, CurrentMontage);
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
