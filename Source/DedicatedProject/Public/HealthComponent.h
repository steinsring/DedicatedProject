// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate);
DECLARE_MULTICAST_DELEGATE(FOnHPChangedDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEDICATEDPROJECT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_CurrentHealth();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetHP(float NewHP);

	UFUNCTION(Server, Reliable)
	void ApplyDamage_Server(float DamageAmount);

	float GetHPRatio();

	float GetCurrentHealth() const { return CurrentHealth; }

	float GetMaxHealth() const { return MaxHealth; }

	FOnHPIsZeroDelegate OnHPIsZero; // HP가 0이 되었을 때 호출되는 델리게이트
	FOnHPChangedDelegate OnHPChanged;

private:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, EditAnywhere, Category = "Actor Stat")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Stat")
	float MaxHealth;
};
