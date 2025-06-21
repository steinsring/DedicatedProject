// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_CollisionOnOff.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EHandType :uint8
{
	Left,
	Right
};

UCLASS()
class DEDICATEDPROJECT_API UAnimNotify_CollisionOnOff : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	EHandType HandType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bEnable = true;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
