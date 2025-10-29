// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/PE_PlayerController.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true); // 컴포넌트가 기본적으로 복제되도록 설정

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) // 서버에서만 실행
	{
		CurrentHealth = MaxHealth; // 게임 시작 시 최대 체력으로 초기화
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}

void UHealthComponent::OnRep_CurrentHealth()
{
	OnHPChanged.Broadcast(); // HP가 변경되었음을 알리는 델리게이트 호출
	if (CurrentHealth <= 0.0f)
	{
		CurrentHealth = 0.0f; // HP가 0 이하로 떨어지면 0으로 설정
		OnHPIsZero.Broadcast(); // HP가 0이 되었음을 알리는 델리게이트 호출
	}
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthComponent::SetHP(float NewHP)
{
	if (GetOwnerRole() == ROLE_Authority) // 서버에서만 실행
	{
		CurrentHealth = FMath::Clamp(NewHP, 0.0f, MaxHealth); // HP를 0과 최대 체력 사이로 제한
		OnHPChanged.Broadcast(); // HP가 변경되었음을 알리는 델리게이트 호출
		if (CurrentHealth <= 0.0f)
		{
			CurrentHealth = 0.0f; // HP가 0 이하로 떨어지면 0으로 설정
			OnHPIsZero.Broadcast(); // HP가 0이 되었음을 알리는 델리게이트 호출
		}
	}
}

void UHealthComponent::ApplyDamage_Server_Implementation(float DamageAmount)
{
	if (GetOwnerRole() == ROLE_Authority) // 서버에서만 실행
	{
		SetHP(CurrentHealth - DamageAmount); // HP 감소
	}
}

float UHealthComponent::GetHPRatio()
{
	return (CurrentHealth < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHealth / MaxHealth);
}

