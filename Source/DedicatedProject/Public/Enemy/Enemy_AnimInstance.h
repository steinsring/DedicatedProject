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
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimMontage* GetRandomAttackMontage();

	virtual void PlayStunMontage(float Duration = 1.0f) override;
	virtual void EndStunMontage() override;

	void SetWandering(bool bWandering) { IsWandering = bWandering; }

	bool GetWandering() const { return IsWandering; }

	void SetGoingLeft(bool bLeft) { IsGoingLeft = bLeft; }

	bool GetGoingLeft() const { return IsGoingLeft; }

//protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	bool IsWandering = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	bool IsGoingLeft = true;
};
