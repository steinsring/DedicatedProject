// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/SkillManagerComponent.h"
#include "Player/ProjectPlayer.h"
#include "Camera/CameraComponent.h"
#include "Enemy/PE_AIController.h"
#include "Enemy/Enemy.h"
#include "DedicatedProject.h"


// Sets default values for this component's properties
USkillManagerComponent::USkillManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CurrentAugmentSkill = E_Skills::None;
	CurrentOverrideSkill = E_Skills::None;

	Player = Cast<AProjectPlayer>(GetOwner());

	UDataTable* SkillDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/DataTable/DT_SkillDataTable.DT_SkillDataTable"));
	if (SkillDataTable)
	{
		TArray<FPE_SkillDataTable*> AllSkills;
		SkillDataTable->GetAllRows<FPE_SkillDataTable>(TEXT("SkillData"), AllSkills);
		for (auto* Skill : AllSkills)
		{
			Skills.Add(*Skill);
		}
	}
}


void USkillManagerComponent::UnlockSkill(E_Skills Skill)
{
	int32 SkillIndex = static_cast<int32>(Skill);
	Skills[SkillIndex].bIsUnlocked = true;
	PRINT_LOG(TEXT("Skill %s unlocked!"), *Skills[SkillIndex].SkillName.ToString());
}

void USkillManagerComponent::LockSkill(E_Skills Skill)
{
	int32 SkillIndex = static_cast<int32>(Skill);
	Skills[SkillIndex].bIsUnlocked = true;
	PRINT_LOG(TEXT("Skill %s unlocked!"), *Skills[SkillIndex].SkillName.ToString());
}

void USkillManagerComponent::SetAugmentSkill(E_Skills skill)
{
	CurrentAugmentSkill = skill;
}

void USkillManagerComponent::SetOverrideSkill(E_Skills skill)
{
	CurrentOverrideSkill = skill;
}

bool USkillManagerComponent::IsSkillUnlocked(E_Skills Skill) const
{
	int32 SkillIndex = static_cast<int32>(Skill);
	return Skills[SkillIndex].bIsUnlocked;
}

// Called when the game starts
void USkillManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void USkillManagerComponent::ActivateAugmentSkill(E_Skills skill, float Multiplier, float Duration)
{
	if (Player)
	{
		switch (skill)
		{
		case E_Skills::AttackUp:
			Player->SetAttackPowerMultiplier(Multiplier);
			Player->SetAttackPower(Multiplier);
			break;

		case E_Skills::DefenseUp:
			Player->SetDamageMultiplier(Multiplier);
			break;

		case E_Skills::SpeedUp:
			Player->SetSpeedMultiplier(Multiplier);
			break;

		default:
			break;
		}
	}
}

void USkillManagerComponent::GetHitResultActor(float Distance)
{
	UCameraComponent* PlayerCamera = Player->GetPlayerCamComp();
	if (PlayerCamera == nullptr)
	{
		PRINT_LOG(TEXT("PlayerCamera is null"));
		return;
	}

	FVector Start = Player->GetActorLocation();
	FVector End = Start + (PlayerCamera->GetForwardVector() * Distance);
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Player);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start, 
		End, 
		ECC_Pawn, 
		Params
	);

	FColor LineColor = bHit ? FColor::Red : FColor::Green;
	DrawDebugLine(GetWorld(), Start, End, LineColor, false, 2.0f, 0, 1.0f);

	if (bHit)
	{
		PRINT_LOG(TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Yellow, false, 2.0f);
		HitActor = HitResult.GetActor();
	}

	return;
}

int32 USkillManagerComponent::AttackUp()
{
	int SkillIndex = static_cast<int32>(E_Skills::AttackUp);
	int32 SkillCost = Skills[SkillIndex].Cost;

	float Multiplier = 1.5f;
	PRINT_LOG(TEXT("Attack Up Activated"));
	ActivateAugmentSkill(E_Skills::AttackUp, Multiplier, 10.0f);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Multiplier]()
	{
		if (Player)
		{
			Player->SetAttackPowerMultiplier(1.0f);
			Player->SetAttackPower(Player->GetAttackPower() / Multiplier);
			PRINT_LOG(TEXT("Attack Up Deactivated"));
		}
	}, 10.0f, false);

	return SkillCost;
}

int32 USkillManagerComponent::DefenseUp()
{
	int SkillIndex = static_cast<int32>(E_Skills::DefenseUp);
	int32 SkillCost = Skills[SkillIndex].Cost;

	PRINT_LOG(TEXT("Defense Up Activated"));
	float Multiplier = 0.5f; // 데미지 50% 감소
	ActivateAugmentSkill(E_Skills::DefenseUp, Multiplier, 10.0f);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Multiplier]()
	{
		if (Player)
		{
			Player->SetDamageMultiplier(1.0f);
			PRINT_LOG(TEXT("Defense Up Deactivated"));
		}
	}, 10.0f, false);

	return SkillCost;
}

int32 USkillManagerComponent::SpeedUp()
{
	int SkillIndex = static_cast<int32>(E_Skills::SpeedUp);
	int32 SkillCost = Skills[SkillIndex].Cost;

	PRINT_LOG(TEXT("Speed Up Activated"));
	float Multiplier = 1.5f; // 이동속도 50% 증가
	ActivateAugmentSkill(E_Skills::SpeedUp, Multiplier, 10.0f);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Multiplier]()
	{
		if (Player)
		{
			Player->SetSpeedMultiplier(1.0f);
			PRINT_LOG(TEXT("Speed Up Deactivated"));
		}
	}, 10.0f, false);

	return SkillCost;
}

int32 USkillManagerComponent::SightHacking()
{
	int SkillIndex = static_cast<int32>(E_Skills::SightHacking);
	int32 SkillCost = Skills[SkillIndex].Cost;

	PRINT_LOG(TEXT("Sight Hacking Activated"));

	if (!HitActor)
	{
		PRINT_LOG(TEXT("No Actor Hit"));
		return 0;
	}

	AEnemy* HitEnemy = Cast<AEnemy>(HitActor);
	if (!HitEnemy)
	{
		PRINT_LOG(TEXT("Hit Actor is not an Enemy"));
		return 0;
	}

	APE_AIController* EnemyController = Cast<APE_AIController>(HitEnemy->GetController());
	if (!EnemyController)
	{
		PRINT_LOG(TEXT("EnemyController is null"));
		return 0;
	}

	EnemyController->DisableDetect(10.0f);
	return SkillCost;
}

int32 USkillManagerComponent::Slow()
{
	int SkillIndex = static_cast<int32>(E_Skills::Slow);
	int32 SkillCost = Skills[SkillIndex].Cost;

	PRINT_LOG(TEXT("Slow Activated"));
	return SkillCost;
}

int32 USkillManagerComponent::ElectricShock()
{
	int SkillIndex = static_cast<int32>(E_Skills::ElectricShock);
	int32 SkillCost = Skills[SkillIndex].Cost;

	PRINT_LOG(TEXT("Electric Shock Activated"));
	//GetHitResultActor(1000.0f);

	if (!HitActor)
	{
		PRINT_LOG(TEXT("No Actor Hit"));
		return 0;
	}

	AEnemy* HitEnemy = Cast<AEnemy>(HitActor);
	if (!HitEnemy)
	{
		PRINT_LOG(TEXT("Hit Actor is not an Enemy"));
		return 0;
	}

	HitEnemy->ApplyStun(10.0f);
	return SkillCost;
}

int32 USkillManagerComponent::SeeThrough()
{
	int SkillIndex = static_cast<int32>(E_Skills::SeeThrough);
	int32 SkillCost = Skills[SkillIndex].Cost;

	//PRINT_LOG(TEXT("See Through Activated"));
	return SkillCost;
}
