// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PE_AnimInstance.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"

#include "DedicatedProject.h"

UPE_AnimInstance::UPE_AnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> BasicAttackPattern
	(TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Animations/Fire_A_Slow_Montage.Fire_A_Slow_Montage"));
	if (BasicAttackPattern.Succeeded())
	{
		PRINT_LOG(TEXT("BasicAttackPattern is loaded successfully"));
		BasicAttack = BasicAttackPattern.Object;
		AttackMontages.Add(BasicAttack);
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("BasicAttackPattern is NULL"));
	}
}

void UPE_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AActor* Owner = TryGetPawnOwner();
	FString OwnerName = Owner ? Owner->GetName() : TEXT("Unknown");
	PRINT_LOG(TEXT("My Character name : %s"), *OwnerName);
}

void UPE_AnimInstance::SetPowerState(bool bNewPowerState)
{
	bIsPowerOn = bNewPowerState;
	PRINT_LOG(TEXT("Power State changed: %s"), bIsPowerOn ? TEXT("ON") : TEXT("OFF"));
}
