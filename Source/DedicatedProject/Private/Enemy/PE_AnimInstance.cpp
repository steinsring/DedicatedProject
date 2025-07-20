// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/PE_AnimInstance.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"

#include "DedicatedProject.h"

UPE_AnimInstance::UPE_AnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> BasicAttackPattern
	(TEXT("/Game/Retargetting/SK_Mannequin_Skeleton_BasicAttack_Montage.SK_Mannequin_Skeleton_BasicAttack_Montage"));
	if (BasicAttackPattern.Succeeded())
	{
		PRINT_LOG(TEXT("BasicAttackPattern is loaded successfully"));
		BasicAttack = BasicAttackPattern.Object;
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("BasicAttackPattern is NULL"));
	}
}