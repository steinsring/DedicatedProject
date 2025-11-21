// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/ToiletMech_AnimInstance.h"
#include "Enemy/ToiletMechTest.h"
#include "Weapon/PE_WeaponProjectileComponent.h"
#include "DedicatedProject.h"

//공격 패턴만 넣어주면 됨
UToiletMech_AnimInstance::UToiletMech_AnimInstance()
{
	// 공격 몽타주를 불러오기
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontage1
	(TEXT("/Game/Fab/SciFi_ToiletMech/Animation/SKEL_SciFi_ToiletMech_Skeleton_Montage.SKEL_SciFi_ToiletMech_Skeleton_Montage"));
	if (AttackMontage1.Succeeded())
	{
		AttackPattern1 = AttackMontage1.Object;
	}
	AttackMontages.Add(AttackPattern1);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontage2
	(TEXT("/Game/Fab/SciFi_ToiletMech/Animation/ToiletMech_Attack2_Montage.ToiletMech_Attack2_Montage"));
	if (AttackMontage2.Succeeded())
	{
		AttackPattern2 = AttackMontage2.Object;
	}
	AttackMontages.Add(AttackPattern2);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontage3
	(TEXT("/Game/Fab/SciFi_ToiletMech/Animation/ToiletMech_Attack3_Montage.ToiletMech_Attack3_Montage"));
	if (AttackMontage3.Succeeded())
	{
		AttackPattern3 = AttackMontage3.Object;
	}
	AttackMontages.Add(AttackPattern3);
	
	PRINT_LOG(TEXT("ToiletMech AttackMontage Count: %d"), AttackMontages.Num());

	// 스턴 몽타주 할당
	static ConstructorHelpers::FObjectFinder<UAnimMontage> TempMontage
	(TEXT("/Game/Fab/SciFi_ToiletMech/Animation/Anim_SciFi_ToiletMech_Stun_Montage.Anim_SciFi_ToiletMech_Stun_Montage"));
	if (TempMontage.Succeeded())
	{
		StunMontage = TempMontage.Object;
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("StunMontage is Null"));
	}
}

void UToiletMech_AnimInstance::AnimNotify_ToiletMechShoot()
{
	auto ToiletMech = Cast<AToiletMechTest>(TryGetPawnOwner());
	if (!ToiletMech) return;

	if (!ToiletMech->HasAuthority()) return;

	ToiletMech->GetWeaponProjectileComponent()->Fire_Enemy();
}