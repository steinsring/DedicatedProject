// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Common_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UCommon_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UCommon_AnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	void PlayAttackMontage(UAnimMontage* AnimMontage);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", meta = (AllowPrivateAccess = true))
	bool IsInAir;
};
