// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy_AnimInstance.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"

#include "DedicatedProject.h"

UEnemy_AnimInstance::UEnemy_AnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
}

//Tick마다 호출되는 AnimInstance함수
void UEnemy_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	//pawn을 상속받는 character가 불러와짐(ProjectPlayer)
	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		//UE_LOG(LogTemp, Log, TEXT("Speed : %f"), CurrentPawnSpeed);
		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}
}

void UEnemy_AnimInstance::PlayAttackMontage(UAnimMontage* AnimMontage)
{
	if (!AnimMontage)
	{
		PRINT_LOG(TEXT("AnimMontage is NULL"));
		return;
	}
	Montage_Play(AnimMontage, 1.0f);
}

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
