// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class DEDICATEDPROJECT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	UDataTable* EnemyDataTable;

	float AttackPower = 0.0f;
	TSet<AActor*> HitActors;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

public:
	void Attack(UAnimMontage* AnimMontage);
	FOnAttackEndDelegate OnAttackEnd;

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void AttackCheck();

	UFUNCTION()
	void OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//블루프린트에서 해주기
	UFUNCTION(BlueprintCallable)
	void SetHitbox(ECollisionEnabled::Type CollisionEnabled, UCapsuleComponent* HitBox);
	
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY()
	class UEnemy_AnimInstance* EnemyAnim;

	UPROPERTY(VisibleAnywhere)
	UHealthComponent* HealthComp;
};
