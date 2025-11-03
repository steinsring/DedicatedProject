// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PE_ItemSpawner.generated.h"

UCLASS()
class DEDICATEDPROJECT_API APE_ItemSpawner : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, Category = "Spawn System")
	TArray<TSubclassOf<class APE_ItemPickup>> SpawnableItems;

public:	
	// Sets default values for this actor's properties
	APE_ItemSpawner();

	UFUNCTION()
	void Spawn(const FVector Location, const FRotator Rotation);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
