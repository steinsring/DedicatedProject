// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTree/BTTask_Attack.h"
#include "DedicatedProject.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/PE_AIController.h"
#include "Enemy/Enemy_AnimInstance.h"
#include "Enemy/Enemy.h"

UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;
	IsAttacking = false;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == Enemy)
		return EBTNodeResult::Failed;
	auto EnemyAnimInstance = Cast<UEnemy_AnimInstance>(Enemy->GetMesh()->GetAnimInstance());
	if (nullptr == EnemyAnimInstance)
	{
		PRINT_LOG(TEXT("EnemyAnimInstance is NULL"));
		return EBTNodeResult::Failed;
	}
	auto RandomMontage = EnemyAnimInstance->GetRandomAttackMontage();
	if (nullptr == RandomMontage)
	{
		PRINT_LOG(TEXT("RandomMontage is NULL"));
		return EBTNodeResult::Failed;
	}

	Enemy->Attack(RandomMontage);
	IsAttacking = true;
	Enemy->OnAttackEnd.AddLambda([this]()  -> void {
		IsAttacking = false;
	});

	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsAttacking)
	{
		UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
		BB->SetValueAsBool(TEXT("bShouldWander"), true);

		//auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
		//if (nullptr == Enemy) return;

		//auto EnemyAnimInstance = Cast<UEnemy_AnimInstance>(Enemy->GetMesh()->GetAnimInstance());
		//if (nullptr == EnemyAnimInstance)
		//{
		//	PRINT_LOG(TEXT("EnemyAnimInstance is NULL"));
		//	return;
		//}
		//if (EnemyAnimInstance)
		//{
		//	EnemyAnimInstance->SetWandering(true);
		//	if (EnemyAnimInstance->GetWandering())
		//	{
		//		PRINT_LOG(TEXT("Wandering true"));
		//	}
		//	else
		//	{
		//		PRINT_LOG(TEXT("Wandering false"));
		//	}
		//}

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
