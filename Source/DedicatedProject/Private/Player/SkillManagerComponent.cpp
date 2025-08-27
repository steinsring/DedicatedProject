// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/SkillManagerComponent.h"
#include "DedicatedProject.h"


// Sets default values for this component's properties
USkillManagerComponent::USkillManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

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
