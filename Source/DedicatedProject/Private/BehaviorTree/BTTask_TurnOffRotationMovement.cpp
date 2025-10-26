// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTTask_TurnOffRotationMovement.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "DedicatedProject.h"


UBTTask_TurnOffRotationMovement::UBTTask_TurnOffRotationMovement()
{
	NodeName = (TEXT("TurnOffRotation"));
}

EBTNodeResult::Type UBTTask_TurnOffRotationMovement::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
	if (!Pawn) return EBTNodeResult::Failed;

	ACharacter* Character = Cast<ACharacter>(Pawn);
	if (Character)
	{
		Character->bUseControllerRotationYaw = true;
		if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
		{
			MoveComp->bOrientRotationToMovement = false;
			PRINT_LOG(TEXT("TurnOffRotationMovement Succeeded"));
		}
	}
	else return EBTNodeResult::Failed;

	return EBTNodeResult::Succeeded;
}
