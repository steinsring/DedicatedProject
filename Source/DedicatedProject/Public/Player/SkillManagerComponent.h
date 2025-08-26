// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/PE_SkillDataTable.h"
#include "SkillManagerComponent.generated.h"


UENUM(BlueprintType)
enum class E_AugmentSkills : uint8
{
	AttackUp		UMETA(DisplayName = "AttackUp"),
	DefenseUp		UMETA(DisplayName = "DefenseUp"),
	SpeedUp			UMETA(DisplayName = "SpeedUp")
};

UENUM(BlueprintType)
enum class E_OverrideSkills : uint8
{
	SightHacking	UMETA(DisplayName = "SightHacking"),
	Slow			UMETA(DisplayName = "Slow"),
	ElectricShock	UMETA(DisplayName = "ElectricShock"),
	SeeThrough		UMETA(DisplayName = "SeeThrough")
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEDICATEDPROJECT_API USkillManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkillManagerComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	TArray<FPE_SkillDataTable> Skills;

	UFUNCTION(BlueprintCallable)
	void UnlockSkill(FName SkillName);

	UFUNCTION(BlueprintCallable)
	void LockSkill(FName SkillName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	E_AugmentSkills CurrentAugmentSkill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	E_OverrideSkills CurrentOverrideSkill;

	UFUNCTION(BlueprintCallable)
	void SetAugmentSkill(E_AugmentSkills skill);

	UFUNCTION(BlueprintCallable)
	void SetOverrideSkill(E_OverrideSkills skill);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
