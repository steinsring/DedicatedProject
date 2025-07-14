// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enemy_AnimInstance.generated.h"

/**
 *
 */
UCLASS()
class DEDICATEDPROJECT_API UEnemy_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UEnemy_AnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage(UAnimMontage* AnimMontage);

public:
	UFUNCTION()
	void AnimNotify_AttackRangeCheck();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimMontage* GetRandomAttackMontage();

protected:
	TArray<UAnimMontage*> AttackMontages;
       
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", meta = (AllowPrivateAccess = true))
	bool IsInAir;
};
