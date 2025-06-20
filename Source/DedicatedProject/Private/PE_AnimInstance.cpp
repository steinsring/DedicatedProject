// Fill out your copyright notice in the Description page of Project Settings.


#include "PE_AnimInstance.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"

UPE_AnimInstance::UPE_AnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Pattern1
	(TEXT("/Script/Engine.AnimMontage'/Game/Fab/SciFi_ToiletMech/Animation/SKEL_SciFi_ToiletMech_Skeleton_Montage.SKEL_SciFi_ToiletMech_Skeleton_Montage'"));
	if (Pattern1.Succeeded())
	{
		AttackPattern1 = Pattern1.Object;
	}

	//static ConstructorHelpers::FObjectFinder<UAnimMontage> Pattern2
	//(TEXT("/Script/Engine.AnimMontage'/Game/Fab/SciFi_ToiletMech/Animation/SKEL_SciFi_ToiletMech_Skeleton_Montage.SKEL_SciFi_ToiletMech_Skeleton_Montage'"));
	//if (Pattern2.Succeeded())
	//{
	//	AttackPattern2 = Pattern2.Object;
	//}
}

//Tick마다 호출되는 AnimInstance함수
void UPE_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
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

void UPE_AnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackPattern1, 1.0f);
}

void UPE_AnimInstance::AnimNotify_AttackRangeCheck()
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
		if (Montage_IsPlaying(AttackPattern1))
		{
			Montage_Stop(0.2f, AttackPattern1);
		}
	}
}

void UPE_AnimInstance::AnimNotify_AttackHitCheck()
{
}
