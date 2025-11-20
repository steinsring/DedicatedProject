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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//블루프린트에서 해주기
	UFUNCTION(BlueprintCallable)
	void SetHitbox(ECollisionEnabled::Type CollisionEnabled, UCapsuleComponent* HitBox);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bIsWandering = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bIsGoingBack = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bIsGoingLeft = false;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetIsWandering(bool bNewState) { bIsWandering = bNewState; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool GetIsWandering() const { return bIsWandering; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetIsGoingBack(bool bNewState);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool GetIsGoingBack() const { return bIsGoingBack; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetIsGoingLeft(bool bNewState);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool GetIsGoingLeft() const { return bIsGoingLeft; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	UStaticMeshComponent* SightHackMesh;

protected:
	UPROPERTY()
	UMaterialInstanceDynamic* MosaicMID = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	int32 EyeMaterialIndex = -1;

	UPROPERTY(ReplicatedUsing = OnRep_SightHacked)
	bool bSightHacked = false;

	UFUNCTION()
	void OnRep_SightHacked();

	void UpdateSightHackeVisual();

public:
	UFUNCTION(Server, Reliable)
	void SetSightHacked_Server(bool bNewState);
	
private:
	UPROPERTY()
	class UEnemy_AnimInstance* EnemyAnim;
};
