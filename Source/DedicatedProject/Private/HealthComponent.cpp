// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/PE_PlayerController.h"
#include "Player/ProjectPlayer.h"
#include "CharacterCommon.h"
#include "Common_AnimInstance.h"
#include "Enemy/Enemy.h"
#include "AIController.h"

#include "DedicatedProject.h"


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

	CurrentHealth = MaxHealth; // 게임 시작 시 최대 체력으로 초기화
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
	CurrentHealth = FMath::Clamp(NewHP, 0.0f, MaxHealth); // HP를 0과 최대 체력 사이로 제한
	OnHPChanged.Broadcast(); // HP가 변경되었음을 알리는 델리게이트 호출

	if (CurrentHealth <= 0.0f)
	{
		PRINT_LOG(TEXT("HP is 0 or below."));
		CurrentHealth = 0.0f; // HP가 0 이하로 떨어지면 0으로 설정
			
		ACharacterCommon* OwnerCharacter = Cast<ACharacterCommon>(GetOwner());
		if (!OwnerCharacter) return;

		UCommon_AnimInstance* CommonAnim = Cast<UCommon_AnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
		if (!CommonAnim) return;

		AEnemy* OwnerEnemy = Cast<AEnemy>(OwnerCharacter);
		if (OwnerEnemy)
		{
			AAIController* AIController = Cast<AAIController>(OwnerEnemy->GetController());
			if (AIController && AIController->BrainComponent)
			{
				AIController->UnPossess();
			}
		}
		CommonAnim->SetIsDead(true);

		AProjectPlayer* Player = Cast<AProjectPlayer>(OwnerCharacter);
		if (Player) Player->Die();

		OwnerCharacter->SetIsDead(true);
		OwnerCharacter->HandleDeath();
	}
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	PRINT_LOG(TEXT("Current %s Health: %f"), *OwnerCharacter->GetName(), CurrentHealth);
}

//void UHealthComponent::ApplyDamage_Server_Implementation(float DamageAmount)
//{
//	ApplyDamage_Multicast(DamageAmount); // HP 감소
//}
//
//void UHealthComponent::ApplyDamage_Multicast_Implementation(float DamageAmount)
//{
//	SetHP(CurrentHealth - DamageAmount); // HP 감소
//}

void UHealthComponent::ApplyDamage(float DamageAmount)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		ApplyDamage_Internal(DamageAmount);
	}
	else
	{
		PRINT_LOG(TEXT("ApplyDamage called on Client, ignored"));
	}
}

void UHealthComponent::ApplyDamage_Internal(float DamageAmount)
{
	if (DamageAmount <= 0.0f) return;

	SetHP(CurrentHealth - DamageAmount); // HP 감소
}

float UHealthComponent::GetHPRatio()
{
	return (CurrentHealth < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHealth / MaxHealth);
}

