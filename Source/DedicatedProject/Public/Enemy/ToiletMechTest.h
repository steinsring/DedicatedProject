// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "ToiletMechTest.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API AToiletMechTest : public AEnemy
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	AToiletMechTest();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hitbox")
	UCapsuleComponent* LeftHandHitBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hitbox")
	UCapsuleComponent* RightHandHitBox;
};
