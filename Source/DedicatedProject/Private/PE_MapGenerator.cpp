// Fill out your copyright notice in the Description page of Project Settings.

#include "DedicatedProject.h"
#include "PE_MapGenerator.h"
#include "Components/BoxComponent.h"

// Sets default values
APE_MapGenerator::APE_MapGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 맵을 생성할 위치 설정
	GenerateArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Generate Area"));
	GenerateArea->SetupAttachment(RootComponent);
	GenerateArea->SetBoxExtent(FVector(MapSizeX, MapSizeY, MapSizeZ));

}

// Called when the game starts or when spawned
void APE_MapGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	if (GeneratableMaps.IsEmpty())
		return;
	
	if (GetLocalRole() != ROLE_Authority)
		return;

	// BSP 알고리즘
	RootNode = MakeBSPNode();
	RootNode->Split(RootNode, MaxRoomSize);

	GenerateMap();
}

// Called every frame
void APE_MapGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TSharedPtr<FBSPNode> APE_MapGenerator::MakeBSPNode() {
	FVector WorldLocation = GenerateArea->GetComponentLocation(); // boxcomponent transform
	FVector2D MapMinCoordinate = FVector2D(WorldLocation.X - MapSizeX, WorldLocation.Y - MapSizeY);
	FVector2D MapMaxCoordinate = FVector2D(WorldLocation.X + MapSizeX, WorldLocation.Y + MapSizeY);
	TSharedPtr<FBSPNode> Node = MakeShared<FBSPNode>(FBSPNode{ MapMinCoordinate, MapMaxCoordinate });
	return Node;
}

void APE_MapGenerator::CollectLeaves(const TSharedPtr<FBSPNode>& Node, TArray<TSharedPtr<FBSPNode>>& OutLeaves)
{
	if (!Node.IsValid())
		return;

	if (Node->IsLeaf())
	{
		OutLeaves.Add(Node);
		return;
	}

	CollectLeaves(Node->Left, OutLeaves);
	CollectLeaves(Node->Right, OutLeaves);
}


void APE_MapGenerator::GenerateMap() {
	FActorSpawnParameters SpawnParams; // Actor를 생성할 때 사용할 파라미터를 담는 구조체
	//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding; // 스폰하려는 레벨의 다른 오브젝트가 있다면 위치를 조정. 그래도 충돌하면 스폰하지 않음.

	CollectLeaves(RootNode, LeavesList);

	auto Map = GeneratableMaps[FMath::RandRange(0, GeneratableMaps.Num() - 1)]; // 저장된 배열안의 StaticMesh중 랜덤하게 하나 골라서

	// GenerateArea 영역 안의 랜덤 위치에 랜덤 방향으로 Spawn
	const auto Rotation = FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f); //Z축(Yaw) 회전을 랜덤하게

	for (auto& Leaf : LeavesList) {
		const auto Center = FVector((Leaf->MaxCoordinate + Leaf->MinCoordinate) * 0.5f, 0.0f);
		const auto Location = Center;
		PRINT_LOG(TEXT("My Log : %s "), *Center.ToString());
		GetWorld()->SpawnActor<AActor>(Map, Location, Rotation, SpawnParams); // Actor를 맵에 Spawn
	}
		
	/*
	const auto Location = GenerateArea->GetComponentLocation() +
		FVector(FMath::RandRange(-GenerateArea->GetScaledBoxExtent().X, GenerateArea->GetScaledBoxExtent().X),
				FMath::RandRange(-GenerateArea->GetScaledBoxExtent().Y, GenerateArea->GetScaledBoxExtent().Y),
				0.0f); // 박스 콜라이더 범위 내 랜덤한 X, Y좌표를 생성
	*/
}

