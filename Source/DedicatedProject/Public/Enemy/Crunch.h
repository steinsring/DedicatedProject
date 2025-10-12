// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "Crunch.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API ACrunch : public AEnemy
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	ACrunch();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitBox")
	UCapsuleComponent* LeftHandHitBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitBox")
	UCapsuleComponent* RightHandHitBox;
};
