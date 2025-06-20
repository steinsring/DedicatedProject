// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CurrentHealth = 100.0f;
	MaxHealth = 100.0f;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UHealthComponent::ApplyDamage(float DamageAmount)
{
	CurrentHealth -= DamageAmount;
	UE_LOG(LogTemp, Log, TEXT("Health Remain : %f"), CurrentHealth);
	if (CurrentHealth <= 0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("You Died"));
	}
	return DamageAmount;
}

