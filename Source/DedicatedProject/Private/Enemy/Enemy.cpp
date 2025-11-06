// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"
#include "Enemy/PE_AIController.h"
#include "Enemy/Enemy_AnimInstance.h"

#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"

#include "Enemy/PE_ToiletMechStats.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ProjectPlayer.h"

#include "DedicatedProject.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	AProjectPlayer* Player = Cast<AProjectPlayer>(DamageCauser);
	if (Player)
	{
		float DamageMultiplier = Player->GetAttackPowerMultiplier();
		float CalculatedDamage = DamageAmount * DamageMultiplier;
		HealthComp->ApplyDamage(CalculatedDamage);

		return CalculatedDamage;
	}

	return 0.0f;
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}