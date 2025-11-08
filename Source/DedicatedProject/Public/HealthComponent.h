// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

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

	//UFUNCTION(Server, Reliable)
	//void ApplyDamage_Server(float DamageAmount);

	//UFUNCTION(NetMulticast, Reliable)
	//void ApplyDamage_Multicast(float DamageAmount);

	void ApplyDamage_Internal(float DamageAmount);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetHP(float NewHP);

	void ApplyDamage(float DamageAmount);

	float GetHPRatio();

	float GetCurrentHealth() const { return CurrentHealth; }

	float GetMaxHealth() const { return MaxHealth; }

	FOnHPChangedDelegate OnHPChanged;

private:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, EditAnywhere, Category = "Actor Stat")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Stat")
	float MaxHealth;
};
