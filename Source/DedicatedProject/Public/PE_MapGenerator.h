// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PE_MapGenerator.generated.h"

UCLASS()
class DEDICATEDPROJECT_API APE_MapGenerator : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generater System", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> GenerateArea; //스폰될 맵의 전체 크기

public:	
	// Sets default values for this actor's properties
	APE_MapGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void GenerateMap();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	TArray<TSubclassOf<class AActor>> GeneratableMaps;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	int32 NumMapsAtStart = 5;
};
