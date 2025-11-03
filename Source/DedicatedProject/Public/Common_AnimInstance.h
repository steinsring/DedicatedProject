// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Common_AnimInstance.generated.h"

enum class EMontageType { Attack, Stun, Dead, None };

UCLASS()
class DEDICATEDPROJECT_API UCommon_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UCommon_AnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	void PlayMontage(UAnimMontage* AnimMontage);

	virtual void PlayStunMontage(float Duration = 0.0);
	virtual void EndStunMontage();

	EMontageType GetMontageType(UAnimMontage* Montage) const;

	void SetIsDead(bool bDead) { IsDead = bDead; }

	bool GetIsDead() const { return IsDead; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "Stun", meta = (AllowPrivateAccess = true))
	UAnimMontage* StunMontage = nullptr;	// 애니메이션 몽타주는 부모에서 선언하고 자식에서 각각 할당함.

	UPROPERTY(VisibleAnywhere, Category = "Stun", meta = (AllowPrivateAccess = true))
	bool IsStuned = false;

	float StunDuration = 0.0;

	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackPattern1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackPattern2;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn", meta = (AllowPrivateAccess = true))
	bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn", meta = (AllowPrivateAccess = true))
	bool IsDead;
};
