// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "PlayerAI.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API APlayerAI : public AEnemy
{
	GENERATED_BODY()
	
	
public:
	// Sets default values for this character's properties
	APlayerAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	class UPE_WeaponProjectileComponent* GetWeaponProjectileComponent() const { return WeaponProjectileComponent; }

private:
	UPROPERTY(VisibleAnywhere, Category = "WeaponProjectile")
	TObjectPtr<class UPE_WeaponProjectileComponent> WeaponProjectileComponent;
	
};
