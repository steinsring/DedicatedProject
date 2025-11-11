// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "SpawnPoint.generated.h"

UENUM(BlueprintType)
enum class E_EnemyType : uint8
{
	ToiletMech		UMETA(DisplayName = "ToiletMech"),
	Crunch		    UMETA(DisplayName = "Crunch"),
	Player			UMETA(DisplayName = "Player"),
	Boss		    UMETA(DisplayName = "Boss"),
	Minion		    UMETA(DisplayName = "Minion")
};

UCLASS()
class DEDICATEDPROJECT_API ASpawnPoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:
	ASpawnPoint();

	void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnPoint")
	E_EnemyType EnemyType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnPoint")
	TArray<ATargetPoint*> WayPoints;
};
