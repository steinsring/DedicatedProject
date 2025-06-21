// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PE_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UPE_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPE_AnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage();

public:
	UFUNCTION()
	void AnimNotify_AttackRangeCheck();
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", meta = (AllowPrivateAccess=true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", meta = (AllowPrivateAccess = true))
	bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackPattern1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackPattern2;
};
