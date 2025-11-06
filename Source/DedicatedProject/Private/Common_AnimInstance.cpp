// Fill out your copyright notice in the Description page of Project Settings.


#include "Common_AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Enemy/Enemy.h"
#include "Enemy/ToiletMechTest.h"

#include "DedicatedProject.h"

UCommon_AnimInstance::UCommon_AnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;
}

void UCommon_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	//pawn을 상속받는 character가 불러와짐(ProjectPlayer)
	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		//if(Cast<AToiletMechTest>(Pawn)) PRINT_LOG(TEXT("Speed : %f"), CurrentPawnSpeed);
		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}

	//if (::IsValid(Pawn))
	//{
	//	FVector Velocity = Pawn->GetVelocity();
	//	if (Cast<AEnemy>(Pawn))
	//		PRINT_LOG(TEXT("%s | Role:%d | Vel:(%.1f, %.1f, %.1f) | Speed:%.1f"),
	//			*Pawn->GetName(),
	//			(int32)Pawn->GetLocalRole(),
	//			Velocity.X, Velocity.Y, Velocity.Z,
	//			Velocity.Size());
	//}
}

void UCommon_AnimInstance::PlayMontage(UAnimMontage* AnimMontage)
{
	if (!AnimMontage)
	{
		PRINT_LOG(TEXT("AnimMontage is NULL"));
		return;
	}
	Montage_Play(AnimMontage, 1.0f);
}

// 자식에서 구현
void UCommon_AnimInstance::PlayStunMontage(float Duration)
{
	
}

// 자식에서 구현
void UCommon_AnimInstance::EndStunMontage()
{

}

EMontageType UCommon_AnimInstance::GetMontageType(UAnimMontage* Montage) const
{
	if (IsDead)
	{
		return EMontageType::Dead;
	}

	if (AttackMontages.Contains(Montage))
	{
		return EMontageType::Attack;
	}

	if (StunMontage == Montage)
	{
		return EMontageType::Stun;
	}

	return EMontageType::None;
}