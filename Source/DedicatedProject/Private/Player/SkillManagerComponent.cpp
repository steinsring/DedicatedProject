// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/SkillManagerComponent.h"
#include "Player/ProjectPlayer.h"
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


void USkillManagerComponent::UnlockSkill(FName SkillName)
{
	for (auto& Skill : Skills)
	{
		FName SkillNameToName = FName(Skill.SkillName.ToString());
		if (SkillNameToName == SkillName)
		{
			Skill.bIsUnlocked = true;
			PRINT_LOG(TEXT("Skill %s unlocked!"), *SkillName.ToString());
			return; // Exit after unlocking the skill
		}
	}
}

void USkillManagerComponent::LockSkill(FName SkillName)
{
	for (auto& Skill : Skills)
	{
		FName SkillNameToName = FName(Skill.SkillName.ToString());
		if (SkillNameToName == SkillName)
		{
			Skill.bIsUnlocked = false;
			return; // Exit after locking the skill
		}
	}
}

void USkillManagerComponent::SetAugmentSkill(E_Skills skill)
{
	CurrentAugmentSkill = skill;
}

void USkillManagerComponent::SetOverrideSkill(E_Skills skill)
{
	CurrentOverrideSkill = skill;
}

bool USkillManagerComponent::IsSkillUnlocked(FName SkillName) const
{
	for (const auto& Skill : Skills)
	{
		FName SkillNameToName = FName(Skill.SkillName.ToString());
		if (SkillNameToName == SkillName)
		{
			return Skill.bIsUnlocked;
		}
	}
	return false;
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

void USkillManagerComponent::AttackUp()
{
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
}

void USkillManagerComponent::DefenseUp()
{
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
}

void USkillManagerComponent::SpeedUp()
{
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
}

void USkillManagerComponent::SightHacking()
{
	PRINT_LOG(TEXT("Sight Hacking Activated"));
}

void USkillManagerComponent::Slow()
{
	PRINT_LOG(TEXT("Slow Activated"));
}

void USkillManagerComponent::ElectricShock()
{
	PRINT_LOG(TEXT("Electric Shock Activated"));
}

void USkillManagerComponent::SeeThrough()
{
	PRINT_LOG(TEXT("See Through Activated"));
}
