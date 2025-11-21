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

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontage2
	(TEXT("/Game/ParagonCrunch/Characters/Heroes/Crunch/Animations/Attack2_Montage.Attack2_Montage"));
	if (AttackMontage2.Succeeded())
	{
		AttackPattern2 = AttackMontage2.Object;
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("AttackMontage2 is NULL"));
	}
	AttackMontages.Add(AttackPattern2);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontage3
	(TEXT("/Game/ParagonCrunch/Characters/Heroes/Crunch/Animations/Attack2_Montage.Attack2_Montage"));
	if (AttackMontage3.Succeeded())
	{
		AttackPattern3 = AttackMontage3.Object;
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("AttackMontage2 is NULL"));
	}
	AttackMontages.Add(AttackPattern3);

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