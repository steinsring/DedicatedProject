// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FBSPNode.h"
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

	TSharedPtr<FBSPNode> MakeBSPNode();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	TArray<TSubclassOf<class AActor>> GeneratableMaps; // 만들어질 방들 배열

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	int32 NumMapsAtStart = 5; // 만들어질 방의 개수

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	double MapSizeX = 5000.0f; // 전체 맵 크기 반지름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	double MapSizeY = 5000.0f; // 전체 맵 크기 반지름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	double MapSizeZ = 1500.0f; // 전체 맵 크기 반지름

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	FVector2D MaxRoomSize = FVector2D (200.0f, 200.0f); // 최대 방 크기
};
