// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/PlayerAI_AnimInstance.h"
#include "Enemy/PlayerAI.h"
#include "Weapon/PE_WeaponProjectileComponent.h"
#include "DedicatedProject.h"

UPlayerAI_AnimInstance::UPlayerAI_AnimInstance()
{
	// 공격 몽타주를 불러오기
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontage1
	(TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Animations/Attack1_Montage.Attack1_Montage"));
	if (AttackMontage1.Succeeded())
	{
		AttackPattern1 = AttackMontage1.Object;
	}
	AttackMontages.Add(AttackPattern1);

	// 스턴 몽타주 할당
	static ConstructorHelpers::FObjectFinder<UAnimMontage> TempMontage
	(TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Animations/Stun_Montage.Stun_Montage"));
	if (TempMontage.Succeeded())
	{
		StunMontage = TempMontage.Object;
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("StunMontage is Null"));
	}
}

void UPlayerAI_AnimInstance::AnimNotify_PlayerAIShoot()
{
	auto PlayerAI = Cast<APlayerAI>(TryGetPawnOwner());
	if (!PlayerAI) return;

	if (!PlayerAI->HasAuthority()) return;

	PlayerAI->GetWeaponProjectileComponent()->Fire_Enemy();
}
