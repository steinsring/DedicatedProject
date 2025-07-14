// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/ToiletMech_AnimInstance.h"

//공격 패턴만 넣어주면 됨
UToiletMech_AnimInstance::UToiletMech_AnimInstance()
{
	// 공격 몽타주를 불러오기
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontage1
		(TEXT("/Script/Engine.AnimMontage'/Game/Fab/SciFi_ToiletMech/Animation/SKEL_SciFi_ToiletMech_Skeleton_Montage.SKEL_SciFi_ToiletMech_Skeleton_Montage'"));
	if (AttackMontage1.Succeeded())
	{
		AttackPattern1 = AttackMontage1.Object;
	}

	//static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontage2(TEXT("/Game/BluePrints/ToiletMech/Anim/AM_ToiletMech_AttackPattern2.AM_ToiletMech_AttackPattern2"));
	//if (AttackMontage2.Succeeded())
	//{
	//	AttackPattern2 = AttackMontage2.Object;
	//}

	AttackMontages.Add(AttackPattern1);
}
