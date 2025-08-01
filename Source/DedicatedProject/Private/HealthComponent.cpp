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

void UHealthComponent::SetHP(float NewHP)
{
	CurrentHealth = NewHP;
	OnHPChanged.Broadcast(); // HP가 변경되었음을 알리는 델리게이트 호출
	if (CurrentHealth <= 0.0f)
	{
		CurrentHealth = 0.0f; // HP가 0 이하로 떨어지면 0으로 설정
		OnHPIsZero.Broadcast(); // HP가 0이 되었음을 알리는 델리게이트 호출
	}
}

float UHealthComponent::ApplyDamage(float DamageAmount)
{
	AActor* Owner = GetOwner(); // 이 HealthComponent가 달려 있는 액터
	FString OwnerName = Owner ? Owner->GetName() : TEXT("Unknown");

	CurrentHealth -= DamageAmount;
	SetHP(CurrentHealth); // HP를 설정하고 델리게이트 호출
	UE_LOG(LogTemp, Log, TEXT("%s's Health Remain : %f"), *OwnerName, CurrentHealth);

	if (CurrentHealth <= 0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("%s Died"), *OwnerName);
	}

	return DamageAmount;

}

float UHealthComponent::GetHPRatio()
{
	return (CurrentHealth < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHealth / MaxHealth);
}

