// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Crunch_AnimInstance.h"
#include "DedicatedProject.h"

UCrunch_AnimInstance::UCrunch_AnimInstance()
{
	//공격 몽타주
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontage1
	(TEXT("/Game/ParagonCrunch/Characters/Heroes/Crunch/Animations/Attack1_Montage.Attack1_Montage"));
	if (AttackMontage1.Succeeded())
	{
		AttackPattern1 = AttackMontage1.Object;
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("AttackMontage1 is NULL"));
	}
	AttackMontages.Add(AttackPattern1);

	//스턴 몽타주
	static ConstructorHelpers::FObjectFinder<UAnimMontage> TempMontage
	(TEXT("/Game/ParagonCrunch/Characters/Heroes/Crunch/Animations/Stun_Montage.Stun_Montage"));
	if (TempMontage.Succeeded())
	{
		StunMontage = TempMontage.Object;
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("StunMontage is Null"));
	}
}