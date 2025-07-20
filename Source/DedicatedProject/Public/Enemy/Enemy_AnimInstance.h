#pragma once

#include "CoreMinimal.h"
#include "Common_AnimInstance.h" // Include the correct base class header
#include "Enemy_AnimInstance.generated.h"

/**
*
*/
UCLASS()
class DEDICATEDPROJECT_API UEnemy_AnimInstance : public UCommon_AnimInstance // Use UCommon_AnimInstance as base
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void AnimNotify_AttackRangeCheck();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimMontage* GetRandomAttackMontage();

protected:
	TArray<UAnimMontage*> AttackMontages;
};
