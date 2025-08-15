// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SkillManagerComponent.h"


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
		//if (Skill.SkillName == SkillName)
		//{
		//	Skill.bIsUnlocked = true;
		//	return; // Exit after unlocking the skill
		//}
	}
}

void USkillManagerComponent::LockSkill(FName SkillName)
{
	for (auto& Skill : Skills)
	{
		//if (Skill.SkillName == SkillName)
		//{
		//	Skill.bIsUnlocked = false;
		//	return; // Exit after locking the skill
		//}
	}
}

// Called when the game starts
void USkillManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USkillManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USkillManagerComponent::SetSkillData(FPE_SkillDataTable SkillData)
{
	for (auto& Skill : Skills)
	{
		//if (Skill.SkillName == SkillData.SkillName)
		//{
		//	Skill = SkillData;
		//	return; // Exit after setting the skill data
		//}
	}
}

