// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ToiletMech.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class DEDICATEDPROJECT_API AToiletMech : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AToiletMech();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

public:
	void Attack();
	FOnAttackEndDelegate OnAttackEnd;

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void AttackCheck();

	UFUNCTION()
	void OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY()
	class UPE_AnimInstance* PEAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Hitbox")
	UCapsuleComponent* LeftHandHitBox;

	UPROPERTY(EditDefaultsOnly, Category = "Hitbox")
	UCapsuleComponent* RightHandHitBox;
};
