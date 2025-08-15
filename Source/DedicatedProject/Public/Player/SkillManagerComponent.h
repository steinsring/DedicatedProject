// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/PE_SkillDataTable.h"
#include "SkillManagerComponent.generated.h"


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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Skills")
	void SetSkillData(FPE_SkillDataTable SkillData);
};
