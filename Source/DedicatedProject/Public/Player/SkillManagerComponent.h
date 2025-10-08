// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/PE_SkillDataTable.h"
#include "Fuel/PE_FuelComponent.h"
#include "SkillManagerComponent.generated.h"

class AProjectPlayer; // forward declaration

UENUM(BlueprintType)
enum class E_Skills : uint8
{
	AttackUp		UMETA(DisplayName = "AttackUp"),
	DefenseUp		UMETA(DisplayName = "DefenseUp"),
	SpeedUp			UMETA(DisplayName = "SpeedUp"),
	SightHacking	UMETA(DisplayName = "SightHacking"),
	Slow			UMETA(DisplayName = "Slow"),
	ElectricShock	UMETA(DisplayName = "ElectricShock"),
	SeeThrough		UMETA(DisplayName = "SeeThrough"),
	None			UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class E_SkillType : uint8
{
	Augment			UMETA(DisplayName = "Augment"),
	Override		UMETA(DisplayName = "Override")
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEDICATEDPROJECT_API USkillManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USkillManagerComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	TArray<FPE_SkillDataTable> Skills;

	UFUNCTION(BlueprintCallable)
	void UnlockSkill(E_Skills Skill);

	UFUNCTION(BlueprintCallable)
	void LockSkill(E_Skills Skille);

	UFUNCTION(BlueprintCallable)
	void SetAugmentSkill(E_Skills skill);

	UFUNCTION(BlueprintCallable)
	void SetOverrideSkill(E_Skills skill);

	void GetHitResultActor(float Distance);

	UFUNCTION(BlueprintCallable)
	bool IsSkillUnlocked(E_Skills Skill) const;

	FString GetCurrentAugmentSkillName() const
	{
		return *UEnum::GetValueAsString(CurrentAugmentSkill);
	}

	FString GetCurrentOverrideSkillName() const
	{
		return *UEnum::GetValueAsString(CurrentOverrideSkill);
	}

	E_Skills GetCurrentAugmentSkill() const { return CurrentAugmentSkill; }
	E_Skills GetCurrentOverrideSkill() const { return CurrentOverrideSkill; }

	void AttackUp();
	void DefenseUp();
	void SpeedUp();
	void SightHacking();
	void Slow();
	void ElectricShock();
	void SeeThrough();

	int32 GetSkillCost(E_Skills Skill) const;

	//Skill RPC
	UFUNCTION()
	void UseAugmentSkill(E_Skills Skill);

	UFUNCTION(Server, Reliable)
	void UseAugmentSkill_Server(E_Skills Skill);

	UFUNCTION(NetMulticast, Reliable)
	void UseAugmentSkill_Multicast(E_Skills Skill);

	UFUNCTION()
	void UseOverrideSkill(E_Skills Skill);

	UFUNCTION(Server, Reliable)
	void UseOverrideSkill_Server(E_Skills Skill);

	UFUNCTION(NetMulticast, Reliable)
	void UseOverrideSkill_Multicast(E_Skills Skill);

	bool bIsAugmentSkillInCoolTime = false;
	bool bIsOverrideSkillInCoolTime = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	E_Skills CurrentAugmentSkill;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	E_Skills CurrentOverrideSkill;

	void ActivateAugmentSkill(E_Skills skill, float Multiplier);

	AActor* HitActor;
	FHitResult HitResult;
	AProjectPlayer* Player;

	TArray<AActor*> DetectedItems;

	void DetectItems();

	FTimerHandle AugmentSkillCoolTimeTimerHandle;
	FTimerHandle OverrideSkillCoolTimeTimerHandle;

	float AttackUpDuration = 10.0f;
	float DefenseUpDuration = 10.0f;
	float SpeedUpDuration = 10.0f;
	float SeeThroughDuration = 5.0f;

	void StartSkillCoolTime(E_SkillType SkillType, E_Skills Skill);
};
