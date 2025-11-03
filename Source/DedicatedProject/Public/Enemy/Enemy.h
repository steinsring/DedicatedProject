// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "CharacterCommon.h"
#include "Enemy.generated.h"

//DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class DEDICATEDPROJECT_API AEnemy : public ACharacterCommon
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	
private:
	UPROPERTY()
	class UEnemy_AnimInstance* EnemyAnim;
};
