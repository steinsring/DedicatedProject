// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/SpawnPoint.h"
#include "Engine/TargetPoint.h"
#include "WayPoint.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API AWayPoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:
	AWayPoint();

	void SetParentSpawnPoint(ASpawnPoint* SpawnPoint) { ParentSpawnPoint = SpawnPoint; }

	ASpawnPoint* GetParentSpawnPoint() const { return ParentSpawnPoint; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WayPoint")
	ASpawnPoint* ParentSpawnPoint;
};
