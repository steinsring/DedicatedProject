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
	TArray<AActor*> MapList;

	UPROPERTY(VisibleAnywhere, Category = "Generator System")
	TObjectPtr<UStaticMeshComponent> BoxFaces[6];

public:	
	// Sets default values for this actor's properties
	APE_MapGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void GenerateMap();

	//UFUNCTION()
	//TArray<USceneComponent*> FindSceneCompList(AActor* Actor);

	//void SpawnHallway(USceneComponent* Comp, TSharedPtr<FBSPNode> Leaf);

	TSharedPtr<FBSPNode> MakeBSPNode();

	TSharedPtr<FBSPNode> RootNode;

	TArray<TSharedPtr<FBSPNode>> LeavesList;

	void CollectLeaves(const TSharedPtr<FBSPNode>& Node);

	void SetNeighbors(TArray<TSharedPtr<FBSPNode>>& LeavesList);

	bool IsSearchRight(const TSharedPtr<FBSPNode>& Node, const FVector2D& B);

	TSharedPtr<FBSPNode> BinarySearchTree(const TSharedPtr<FBSPNode>& Node, const FVector2D TargetCoordinate);

	AActor* SpawnMap(TSharedPtr<FBSPNode>& Node);

	void SpawnNeighbors(TSharedPtr<FBSPNode>& Node);

	void SpawnEnemies();
private:
	UPROPERTY(VisibleAnywhere, Category = "Spawn Item")
	TObjectPtr<class APE_ItemSpawner> ItemSpawner;				// 아이템 스포너

	void SpawnItems();											// 스폰 위치 계산 후 스포너에게 전달

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	TArray<TSubclassOf<class AActor>> GeneratableMapsStart; // 시작하는 노드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	TArray<TSubclassOf<class AActor>> GeneratableMapsExit4; // 출입구4개
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	TArray<TSubclassOf<class AActor>> GeneratableMapsExit3; // 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	TArray<TSubclassOf<class AActor>> GeneratableMapsExit2_1; // 일자 출입구 2개
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	TArray<TSubclassOf<class AActor>> GeneratableMapsExit2_2; // ㄱ자 출입구 2개
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	TArray<TSubclassOf<class AActor>> GeneratableMapsExit1; // 만들어질 방들 배열

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	//TArray<TSubclassOf<class AActor>> GeneratableBridges; // 만들어질 방들 배열

	/*
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	int32 NumMapsAtStart = 5; // 만들어질 방의 개수
	*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	double MapSizeX = 10000.0f; // 전체 맵 크기 반지름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	double MapSizeY = 10000.0f; // 전체 맵 크기 반지름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	double MapSizeZ = 1500.0f; // 전체 맵 크기 반지름

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generater System")
	FVector2D MaxRoomSize = FVector2D (5000.0f, 5000.0f); // 최대 방 크기
};
