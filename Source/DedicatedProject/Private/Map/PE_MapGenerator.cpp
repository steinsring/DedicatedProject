// Fill out your copyright notice in the Description page of Project Settings.

#include "Map/PE_MapGenerator.h"
#include "DedicatedProject.h"
#include "Components/BoxComponent.h"
#include "PE_GameMode.h"
#include <Kismet/GameplayStatics.h>
#include <Enemy/SpawnPoint.h>
#include <Enemy/ToiletMechTest.h>
#include "Enemy/Crunch.h"
#include "Enemy/PE_AIController.h"
#include "Item/PE_ItemSpawner.h"
#include "Player/ProjectPlayer.h"
#include "GameFramework/CharacterMovementComponent.h" 

// Sets default values
APE_MapGenerator::APE_MapGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 맵을 생성할 위치 설정
	GenerateArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Generate Area"));
	RootComponent = GenerateArea;
	GenerateArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//GenerateArea->SetupAttachment(RootComponent);
	GenerateArea->SetBoxExtent(FVector(MapSizeX, MapSizeY, MapSizeZ));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	for (int32 i = 0; i < 6; i++)
	{
		FString Name = FString::Printf(TEXT("BoxFace_%d"), i);
		BoxFaces[i] = CreateDefaultSubobject<UStaticMeshComponent>(*Name);
		BoxFaces[i]->SetStaticMesh(CubeMesh.Object);
		BoxFaces[i]->SetupAttachment(GenerateArea);
		BoxFaces[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// GenerateArea 크기 가져오기
	FVector Extent = FVector(MapSizeX, MapSizeY, MapSizeZ);

	// 각 면 배치 (앞/뒤/좌/우/위/아래)
	BoxFaces[0]->SetRelativeLocation(FVector(Extent.X, 0, 0));  // +X
	BoxFaces[1]->SetRelativeLocation(FVector(-Extent.X, 0, 0));  // -X
	BoxFaces[2]->SetRelativeLocation(FVector(0, Extent.Y, 0));  // +Y
	BoxFaces[3]->SetRelativeLocation(FVector(0, -Extent.Y, 0));  // -Y
	BoxFaces[4]->SetRelativeLocation(FVector(0, 0, Extent.Z));  // +Z
	BoxFaces[5]->SetRelativeLocation(FVector(0, 0, -Extent.Z));  // -Z

	// 각 면의 크기를 맞추기 (두께는 100 정도로 가정)
	for (int32 i = 0; i < 6; i++)
	{
		FVector Scale = FVector::OneVector;

		if (i < 2) // X축 방향 벽
			Scale = FVector(100.0f, Extent.Y * 2, Extent.Z * 2);
		else if (i < 4) // Y축 방향 벽
			Scale = FVector(Extent.X * 2, 100.0f, Extent.Z * 2);
		else // Z축 방향 벽 (바닥/천장)
			Scale = FVector(Extent.X * 2, Extent.Y * 2, 100.0f);

		BoxFaces[i]->SetWorldScale3D(Scale / 100.0f);
		// ⚠️ Cube 기본 크기가 100이므로 실제 크기 /100
	}

	TArray<FString> BPRoom1Paths = {
		TEXT("/Game/Asset/HomeMade/NodeEntry1/BP_CommonNode1.BP_CommonNode1_C")
	};
	TArray<FString> BPRoom2_1Paths = {
		TEXT("/Game/Asset/HomeMade/NodeEntry2-1/BP_CommonNode2-1-1.BP_CommonNode2-1-1_C")
	};
	TArray<FString> BPRoom2_2Paths = {
		TEXT("/Game/Asset/HomeMade/NodeEntry2-2/BP_CommonNode2-2-1.BP_CommonNode2-2-1_C")
	};
	TArray<FString> BPRoom3Paths = {
		TEXT("/Game/Asset/HomeMade/NodeEntry3/BP_CommonNode3.BP_CommonNode3_C")
	};
	TArray<FString> BPRoom4Paths = {
		TEXT("/Game/Asset/HomeMade/NodeEntry4/BP_CommonNodeEntry4.BP_CommonNodeEntry4_C")
	};

	TArray<FString> BPStartRoomPaths = {
		TEXT("/Game/Asset/HomeMade/NodeEntry4/BP_StartNode1.BP_StartNode1_C")
	};

	for (const FString& Path : BPStartRoomPaths)
	{
		UClass* LoadedClass = LoadClass<AActor>(nullptr, *Path);
		if (LoadedClass)
			GeneratableMapsStart.Add(LoadedClass);
	}

	for (const FString& Path : BPRoom1Paths)
	{
		UClass* LoadedClass = LoadClass<AActor>(nullptr, *Path);
		if (LoadedClass)
			GeneratableMapsExit1.Add(LoadedClass);
	}
	for (const FString& Path : BPRoom2_1Paths)
	{
		UClass* LoadedClass = LoadClass<AActor>(nullptr, *Path);
		if (LoadedClass)
			GeneratableMapsExit2_1.Add(LoadedClass);
	}
	for (const FString& Path : BPRoom2_2Paths)
	{
		UClass* LoadedClass = LoadClass<AActor>(nullptr, *Path);
		if (LoadedClass)
			GeneratableMapsExit2_2.Add(LoadedClass);
	}
	for (const FString& Path : BPRoom3Paths)
	{
		UClass* LoadedClass = LoadClass<AActor>(nullptr, *Path);
		if (LoadedClass)
			GeneratableMapsExit3.Add(LoadedClass);
	}
	for (const FString& Path : BPRoom4Paths)
	{
		UClass* LoadedClass = LoadClass<AActor>(nullptr, *Path);
		if (LoadedClass)
			GeneratableMapsExit4.Add(LoadedClass);
	}

	// 아이템 스포너 생성
	ItemSpawner = CreateDefaultSubobject<APE_ItemSpawner>(TEXT("Item Spawner"));
}

// Called when the game starts or when spawned
void APE_MapGenerator::BeginPlay()
{
	Super::BeginPlay();
	
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

void APE_MapGenerator::CollectLeaves(const TSharedPtr<FBSPNode>& Node)
{// 모든 leaf 수집
	if (!Node.IsValid())
		return;

	if (Node->IsLeaf())
	{
		LeavesList.Add(Node);
		return;
	}

	CollectLeaves(Node->LeftChild);
	CollectLeaves(Node->RightChild);
}

void APE_MapGenerator::SetNeighbors(TArray<TSharedPtr<FBSPNode>>& List)
{
	/*
	각 Leaf에는 노드의 좌하단, 우상단, 정중앙 좌표값이 기록되어 있다.
	모든 Leaf 노드는 크기가 동일하므로 이웃노드와 좌표의 절대값 차이가 동일하다.
	따라서 절대값 차이로 모든 이웃노드의 대략적인 좌하단, 우상단 좌표값을 알 수 있다.
	그리고 좌하단, 우상단 사이에 반드시 정중앙 좌표값이 있을 것이므로 범위 안에 있는 정중앙 좌표값을 가진 노드를
	BinarySearchTree로 검색해 찾아 등록한다.
	*/
	FVector WorldLocation = GenerateArea->GetComponentLocation(); // boxcomponent transform
	FVector2D MapMinCoordinate = FVector2D(WorldLocation.X - MapSizeX, WorldLocation.Y - MapSizeY);
	FVector2D MapMaxCoordinate = FVector2D(WorldLocation.X + MapSizeX, WorldLocation.Y + MapSizeY);

	for (TSharedPtr<FBSPNode>& Leaf : List) {
		FVector2D LeftNodeCenterCoordinate = FVector2D(
			Leaf->CenterCoordinate.X - (Leaf->MaxCoordinate.X - Leaf->MinCoordinate.X),
			Leaf->CenterCoordinate.Y
		);

		if(MapMinCoordinate.ComponentwiseAllLessThan(LeftNodeCenterCoordinate) &&
			LeftNodeCenterCoordinate.ComponentwiseAllLessThan(MapMaxCoordinate))
			Leaf->LeftNode = BinarySearchTree(RootNode, LeftNodeCenterCoordinate);

		FVector2D RightNodeCenterCoordinate = FVector2D(
			Leaf->CenterCoordinate.X + (Leaf->MaxCoordinate.X - Leaf->MinCoordinate.X),
			Leaf->CenterCoordinate.Y
		);

		if (MapMinCoordinate.ComponentwiseAllLessThan(RightNodeCenterCoordinate) && 
			RightNodeCenterCoordinate.ComponentwiseAllLessThan(MapMaxCoordinate))
			Leaf->RightNode = BinarySearchTree(RootNode, RightNodeCenterCoordinate);

		FVector2D TopNodeCenterCoordinate = FVector2D(
			Leaf->CenterCoordinate.X,
			Leaf->CenterCoordinate.Y - (Leaf->MaxCoordinate.Y - Leaf->MinCoordinate.Y)
		);
		if (MapMinCoordinate.ComponentwiseAllLessThan(TopNodeCenterCoordinate) &&
			TopNodeCenterCoordinate.ComponentwiseAllLessThan(MapMaxCoordinate))
			Leaf->TopNode = BinarySearchTree(RootNode, TopNodeCenterCoordinate);

		FVector2D BottomNodeCenterCoordinate = FVector2D(
			Leaf->CenterCoordinate.X,
			Leaf->CenterCoordinate.Y + (Leaf->MaxCoordinate.Y - Leaf->MinCoordinate.Y)
		);
		if (MapMinCoordinate.ComponentwiseAllLessThan(BottomNodeCenterCoordinate) &&
			BottomNodeCenterCoordinate.ComponentwiseAllLessThan(MapMaxCoordinate))
			Leaf->BottomNode = BinarySearchTree(RootNode, BottomNodeCenterCoordinate);
	}
}

bool APE_MapGenerator::IsSearchRight(const TSharedPtr<FBSPNode>& Node, const FVector2D& B)
{
	FVector2D Size = FVector2D(Node->MaxCoordinate - Node->MinCoordinate);
	bool bSplitHorizontally = Size.X < Size.Y;
	if (bSplitHorizontally) // 가로분할한경우 y값으로 판별
		return Node->CenterCoordinate.Y < B.Y;
	else // 세로분할
		return Node->CenterCoordinate.X < B.X;
}

TSharedPtr<FBSPNode> APE_MapGenerator::BinarySearchTree(const TSharedPtr<FBSPNode>& Node, const FVector2D TargetCoordinate) {
	if (Node->IsLeaf())
		return Node;
	else {
		if (IsSearchRight(Node, TargetCoordinate)) // Target이 RightChild에 있는 경우
			return BinarySearchTree(Node->RightChild, TargetCoordinate);
		else 
			return BinarySearchTree(Node->LeftChild, TargetCoordinate);

		return NULL;
	}
}

enum class EDirection { Left, Right, Top, Bottom };

// NeighborNode의 방향을 return 및 abstraction
TSharedPtr<FBSPNode> GetNeighborNode(TSharedPtr<FBSPNode> Node, EDirection Direction)
{
	switch (Direction)
	{
	case EDirection::Left: return Node->LeftNode;
	case EDirection::Right: return Node->RightNode;
	case EDirection::Top: return Node->TopNode;
	case EDirection::Bottom: return Node->BottomNode;
	default: return NULL;
	}
}

// 스폰하려는 노드의 이웃노드와 연결되어있는지를 검사하기 위한 반대 노드 반환
EDirection GetOppositeDirection(EDirection Direction)
{
	switch (Direction)
	{
	case EDirection::Left: return EDirection::Right;
	case EDirection::Right: return EDirection::Left;
	case EDirection::Top: return EDirection::Bottom;
	case EDirection::Bottom: return EDirection::Top;
	default: return Direction;
	}
}

// 방향으로 접근
void SetConnectionFlag(TSharedPtr<FBSPNode> Node, EDirection Direction, bool bConnect)
{
	switch (Direction)
	{
	case EDirection::Left: Node->isConnectLeft = bConnect; break;
	case EDirection::Right: Node->isConnectRight = bConnect; break;
	case EDirection::Top: Node->isConnectTop = bConnect; break;
	case EDirection::Bottom: Node->isConnectBottom = bConnect; break;
	}
}

void CheckAndSetConnection(TSharedPtr<FBSPNode> Node, EDirection Direction)
{
	TSharedPtr<FBSPNode> Neighbor = GetNeighborNode(Node, Direction);
	if (!Neighbor) return; // 이웃이 Null인경우 리턴
	if (!Neighbor->isSpawned) // 이웃이 스폰되지 않은 경우 연결하지 않아도 됨
	{
		SetConnectionFlag(Node, Direction, false);
	}
	else // 스폰된경우 연결해야하는지(이웃노드와 현재노드의 방향이 true인지) 검사
	{
		EDirection OppDirection = GetOppositeDirection(Direction);
		bool bShouldConnect = false;

		switch (OppDirection)
		{
		case EDirection::Left: bShouldConnect = Neighbor->isConnectLeft; break;
		case EDirection::Right: bShouldConnect = Neighbor->isConnectRight; break;
		case EDirection::Top: bShouldConnect = Neighbor->isConnectTop; break;
		case EDirection::Bottom: bShouldConnect = Neighbor->isConnectBottom; break;
		}

		SetConnectionFlag(Node, Direction, bShouldConnect); //연결해야한다면 true
	}
}

enum ERoomExitMask
{
	EXIT_None = 0,
	EXIT_Left = 1 << 0, // 0001
	EXIT_Right = 1 << 1, // 0010
	EXIT_Top = 1 << 2, // 0100
	EXIT_Bottom = 1 << 3  // 1000
};

AActor* APE_MapGenerator::SpawnMap(TSharedPtr<FBSPNode>& Node) {
	/*
	현재 노드를 기준으로 이웃한 노드가 스폰되어 있지 않고 다리로 잇지 않아도 되면
	(`isConnectXXX = false`) 80%확률로 true로 변경해 이을 수 있게 한다.
	그렇게 확정된 `isConnectXXX` 변수에 따라 적절한 방향의 방 에셋을 비트마스킹으로 검사하고 불러와 노드에 스폰한다.
	*/
	if (!Node.IsValid()) return nullptr;

	int32 Mask = 0;
	FVector Location(Node->CenterCoordinate, 0.f);
	TSubclassOf<AActor> Map;
	int32 Rotation;
	
	int32 tempRandom;
	// 스폰되지 않은 방은 80%확률로 true로 변경
	if (Node->LeftNode.IsValid() && !Node->LeftNode->isSpawned) {
		tempRandom = FMath::RandRange(0, 4);
		if (tempRandom > 0)
			Node->isConnectLeft = true;
	}
	if (Node->RightNode.IsValid() && !Node->RightNode->isSpawned) {
		tempRandom = FMath::RandRange(0, 4);
		if (tempRandom > 0)
			Node->isConnectRight = true;
	}
	if (Node->TopNode.IsValid() && !Node->TopNode->isSpawned) {
		tempRandom = FMath::RandRange(0, 4);
		if (tempRandom > 0)
			Node->isConnectTop = true;
	}
	if (Node->BottomNode.IsValid() && !Node->BottomNode->isSpawned) {
		tempRandom = FMath::RandRange(0, 4);
		if (tempRandom > 0)
			Node->isConnectBottom = true;
	}
	
	if (Node->isConnectLeft)   Mask |= EXIT_Left;
	if (Node->isConnectRight)  Mask |= EXIT_Right;
	if (Node->isConnectTop)    Mask |= EXIT_Top;
	if (Node->isConnectBottom) Mask |= EXIT_Bottom;
	if (Mask <= 0 || Mask > 15) {
		PRINT_LOG(TEXT("My Log : %s "), TEXT("Mask is 0 ERROR"));
		Node->isSpawned = true;
		return nullptr;
	}

	AActor* SpawnedMap = nullptr;

	switch (Mask)
	{
	case EXIT_Left:
		Map = GeneratableMapsExit1[FMath::RandRange(0, GeneratableMapsExit1.Num() - 1)];
		SpawnedMap = GetWorld()->SpawnActor<AActor>(Map, Location, FRotator(0, 180, 0));
		Node->isSpawned = true;
		break;
	case EXIT_Right:
		Map = GeneratableMapsExit1[FMath::RandRange(0, GeneratableMapsExit1.Num() - 1)];
		SpawnedMap = GetWorld()->SpawnActor<AActor>(Map, Location, FRotator::ZeroRotator);
		Node->isSpawned = true;
		break;
	case EXIT_Top:
		Map = GeneratableMapsExit1[FMath::RandRange(0, GeneratableMapsExit1.Num() - 1)];
		SpawnedMap = GetWorld()->SpawnActor<AActor>(Map, Location, FRotator(0, -90, 0));
		Node->isSpawned = true;
		break;
	case EXIT_Bottom:
		Map = GeneratableMapsExit1[FMath::RandRange(0, GeneratableMapsExit1.Num() - 1)];
		SpawnedMap = GetWorld()->SpawnActor<AActor>(Map, Location, FRotator(0, 90, 0));
		Node->isSpawned = true;
		break;
	case EXIT_Left | EXIT_Right:
		Map = GeneratableMapsExit2_1[FMath::RandRange(0, GeneratableMapsExit2_1.Num() - 1)];
		SpawnedMap = GetWorld()->SpawnActor<AActor>(Map, Location, FRotator::ZeroRotator);
		Node->isSpawned = true;
		break;
	case EXIT_Left | EXIT_Top:
		Map = GeneratableMapsExit2_2[FMath::RandRange(0, GeneratableMapsExit2_2.Num() - 1)];
		SpawnedMap = GetWorld()->SpawnActor<AActor>(Map, Location, FRotator(0, 180, 0));
		Node->isSpawned = true;
		break;
	case EXIT_Left | EXIT_Bottom:
		Map = GeneratableMapsExit2_2[FMath::RandRange(0, GeneratableMapsExit2_2.Num() - 1)];
		SpawnedMap = GetWorld()->SpawnActor<AActor>(Map, Location, FRotator(0, 90, 0));
		Node->isSpawned = true;
		break;
	case EXIT_Right | EXIT_Top:
		Map = GeneratableMapsExit2_2[FMath::RandRange(0, GeneratableMapsExit2_2.Num() - 1)];
		SpawnedMap = GetWorld()->SpawnActor<AActor>(Map, Location, FRotator(0, -90, 0));
		Node->isSpawned = true;
		break;
	case EXIT_Right | EXIT_Bottom:
		Map = GeneratableMapsExit2_2[FMath::RandRange(0, GeneratableMapsExit2_2.Num() - 1)];
		SpawnedMap = GetWorld()->SpawnActor<AActor>(Map, Location, FRotator::ZeroRotator);
		Node->isSpawned = true;
		break;
	case EXIT_Top | EXIT_Bottom:
		Map = GeneratableMapsExit2_1[FMath::RandRange(0, GeneratableMapsExit2_1.Num() - 1)];
		SpawnedMap = GetWorld()->SpawnActor<AActor>(Map, Location, FRotator(0, 90, 0));
		Node->isSpawned = true;
		break;
	case EXIT_Left | EXIT_Right | EXIT_Top:
		Map = GeneratableMapsExit3[FMath::RandRange(0, GeneratableMapsExit3.Num() - 1)];
		SpawnedMap = GetWorld()->SpawnActor<AActor>(Map, Location, FRotator(0, 180, 0));
		Node->isSpawned = true;
		break;
	case EXIT_Left | EXIT_Right | EXIT_Bottom:
		Map = GeneratableMapsExit3[FMath::RandRange(0, GeneratableMapsExit3.Num() - 1)];
		SpawnedMap = GetWorld()->SpawnActor<AActor>(Map, Location, FRotator::ZeroRotator);
		Node->isSpawned = true;
		break;
	case EXIT_Left | EXIT_Top | EXIT_Bottom:
		Map = GeneratableMapsExit3[FMath::RandRange(0, GeneratableMapsExit3.Num() - 1)];
		SpawnedMap = GetWorld()->SpawnActor<AActor>(Map, Location, FRotator(0, 90, 0));
		Node->isSpawned = true;
		break;
	case EXIT_Right | EXIT_Top | EXIT_Bottom:
		Map = GeneratableMapsExit3[FMath::RandRange(0, GeneratableMapsExit3.Num() - 1)];
		SpawnedMap = GetWorld()->SpawnActor<AActor>(Map, Location, FRotator(0, -90, 0));
		Node->isSpawned = true;
		break;
	case EXIT_Left | EXIT_Right | EXIT_Top | EXIT_Bottom:
		Map = GeneratableMapsExit4[FMath::RandRange(0, GeneratableMapsExit4.Num() - 1)];
		Rotation = 90 * FMath::RandRange(0, 3);
		SpawnedMap = GetWorld()->SpawnActor<AActor>(Map, Location, FRotator(0, Rotation, 0));
		Node->isSpawned = true;
		break;
	default:
		Node->isSpawned = true;
		break;
	}

	return SpawnedMap;
}

// sibal fucking if 문
void APE_MapGenerator::SpawnNeighbors(TSharedPtr<FBSPNode>& Node) {
	for (EDirection Dir : {EDirection::Left, EDirection::Right, EDirection::Top, EDirection::Bottom})
	{// 현재노드의 모든 방향 탐색
		TSharedPtr<FBSPNode> Neighbor = GetNeighborNode(Node, Dir);
		if (!Neighbor) continue; //이웃이 NULL인경우 스폰할 필요 없음

		if (!Neighbor->isSpawned) // 이웃에 스폰되지 않은경우
		{
			for (EDirection SubDir : {EDirection::Left, EDirection::Right, EDirection::Top, EDirection::Bottom})
			{ // 모든 방향의 이웃과 연결 탐색
				CheckAndSetConnection(Neighbor, SubDir);
			}
			MapList.Add(SpawnMap(Neighbor));
			SpawnNeighbors(Neighbor); // 재귀적 확장
		}
		else // 이미스폰된경우 할필요없음
		{
			UE_LOG(LogTemp, Log, TEXT("My Log : Already Spawned, %s"), *Neighbor->CenterCoordinate.ToString());
		}
	}
}

void APE_MapGenerator::SpawnEnemies()
{
	for (auto* Map : MapList)
	{
		if (!Map) continue;

		// Map 액터에서 ChildActorComponent를 가져옴
		TArray<UChildActorComponent*> ChildActorComps;
		Map->GetComponents<UChildActorComponent>(ChildActorComps); // ✅ Map 액터에서 가져오기

		PRINT_LOG(TEXT("My Log : %s %d"), TEXT("ChildActorComps Count : "), ChildActorComps.Num());
		PRINT_LOG(TEXT("My Log : %s %s"), TEXT("Map Name : "), *Map->GetName());

		for (auto* Comp : ChildActorComps)
		{
			// ChildActorComponent가 SpawnPoint인지 확인
			if (ASpawnPoint* SP = Cast<ASpawnPoint>(Comp->GetChildActor()))
			{
				PRINT_LOG(TEXT("My Log : %s %s"), TEXT("Child Actor is SpawnPoint"), *SP->GetName());

				FVector Location = SP->GetActorLocation();
				FRotator Rotation = SP->GetActorRotation();

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				E_EnemyType EnemyType = SP->EnemyType;
				AActor* Enemy = nullptr;
				switch (EnemyType)
				{
				case E_EnemyType::ToiletMech:
					Enemy = GetWorld()->SpawnActor<AToiletMechTest>(Location, Rotation, SpawnParams);
					break;
				case E_EnemyType::Crunch:
					Enemy = GetWorld()->SpawnActor<ACrunch>(Location, Rotation, SpawnParams);
					break;
				case E_EnemyType::Boss:
					Enemy = GetWorld()->SpawnActor<AToiletMechTest>(Location, Rotation, SpawnParams);
					break;
				case E_EnemyType::Player:
					Enemy = GetWorld()->SpawnActor<AProjectPlayer>(Location, Rotation, SpawnParams);
					if (AProjectPlayer* DummyPlayer = Cast<AProjectPlayer>(Enemy))
					{
						DummyPlayer->SetPowerState_Multicast(false);
					}
					break;
				default:
					Enemy = GetWorld()->SpawnActor<AToiletMechTest>(Location, Rotation, SpawnParams);
					break;
				}

				if (Enemy)
				{
					APE_AIController* AIController = Cast<APE_AIController>(Enemy->GetInstigatorController());
					if (AIController)
					{
						AIController->WayPoints = SP->WayPoints;
					}
				}
			}

			else
			{
				PRINT_LOG(TEXT("My Log : %s "), TEXT("Child Actor is not SpawnPoint"));
			}
		}
	}
}

// 아이템 스폰
void APE_MapGenerator::SpawnItems()
{
	// 모든 맵에서 맵마다 개별 생성
	for (AActor* Map : MapList)
	{
		if (!Map) continue;

		// Map 액터에서 ChildActorComponent를 가져옴
		TArray<USceneComponent*> SceneComps;
		Map->GetComponents<USceneComponent>(SceneComps); // ✅ Map 액터에서 가져오기

		for (USceneComponent* Comp : SceneComps)
		{
			PRINT_LOG(TEXT("My Log : %s %d"), TEXT("USceneComponent Count : "), SceneComps.Num());

			// 태그로 필터
			if (!Comp->ComponentHasTag(TEXT("ItemSpawn"))) continue;
			PRINT_LOG(TEXT("My Log : %s %s"), TEXT("Comp Name : "), *Comp->GetName());

			FVector Location = Comp->GetComponentLocation();
			FRotator Rotation = Comp->GetComponentRotation();

			ItemSpawner->Spawn(Location, Rotation);
		}
	}
}

void APE_MapGenerator::GenerateMap() {
	FActorSpawnParameters SpawnParams; // Actor를 생성할 때 사용할 파라미터를 담는 구조체
	//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding; // 스폰하려는 레벨의 다른 오브젝트가 있다면 위치를 조정. 그래도 충돌하면 스폰하지 않음.

	CollectLeaves(RootNode);				// child가 없는 leaf를 모두 수집
	SetNeighbors(LeavesList);				// 모든 leaf를 순회하며 이웃노드를 등록한다

	// StartNode는 항상 4방향으로
	int32 start = FMath::RandRange(0, LeavesList.Num() - 1);
	TSubclassOf<AActor> Map = GeneratableMapsStart[FMath::RandRange(0, GeneratableMapsStart.Num() - 1)];
	const FRotator Rotation = FRotator::ZeroRotator;
	const FVector Location = FVector(LeavesList[start]->CenterCoordinate, 0.0f);
	AActor* StartNode = GetWorld()->SpawnActor<AActor>(Map, Location, Rotation, SpawnParams);
	MapList.Add(StartNode);
	LeavesList[start]->isSpawned = true;
	LeavesList[start]->isConnectLeft = true;
	LeavesList[start]->isConnectRight = true;
	LeavesList[start]->isConnectTop = true;
	LeavesList[start]->isConnectBottom = true;

	SpawnNeighbors(LeavesList[start]);		// StartNode를 기점으로 모든 노드를 재귀적으로 순환해 노드를 스폰

	APE_GameMode* GameMode = Cast<APE_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->SetTargetLocation(Location);
		GameMode->SetbHasTarget(true);

		// 이미 접속 중인 컨트롤러들 처리 시도
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			GameMode->PlacePawnIfReady(It->Get());
		}
	}

	SpawnEnemies();
	SpawnItems();
}

// 여기부터는 쓰지 않음
/*
TArray<USceneComponent*> APE_MapGenerator::FindSceneCompList(AActor* Actor) {
	USceneComponent* RootComp = Cast<USceneComponent>(Actor->GetRootComponent());
	TArray<USceneComponent*> RootCompChildren;
	RootComp->GetChildrenComponents(true, RootCompChildren); // 루트 컴포넌트의 자식들

	// 루트 컴포넌트의 자식들에서 Socket만 분리
	TArray<USceneComponent*> SceneCompList;
	for (USceneComponent* Comp : RootCompChildren) {
		if (Comp && Comp->GetName().Contains(TEXT("Socket")))
			SceneCompList.Add(Comp);
	}

	return SceneCompList;
}

//소켓에 hallway를 스폰 및 위치조정
void APE_MapGenerator::SpawnHallway(USceneComponent* ParentExitComp, TSharedPtr<FBSPNode> Leaf) {
	
	// 브릿지를 Exit에 스폰
	FTransform RoomSocketTransform = ParentExitComp->GetComponentTransform();
	int32 i = FMath::RandRange(0, GeneratableBridges.Num() - 1);
	AActor* Hallway = GetWorld()->SpawnActor<AActor>(GeneratableBridges[i], RoomSocketTransform);

	if (Hallway) {
		
		// 브릿지의 소켓과 방의 소켓의 위치를 일치
		TArray<USceneComponent*> HallwaySceneCompList = FindSceneCompList(Hallway);

		// 방의 소켓에 붙일 브릿지의 소켓 찾기
		USceneComponent* EntranceSocketComp = nullptr;
		TArray<USceneComponent*> ExitSocketCompList;
		for (USceneComponent* HallwaySceneComp : HallwaySceneCompList) {
			if (HallwaySceneComp && HallwaySceneComp->GetName().Contains(TEXT("EntranceSocket")))
				EntranceSocketComp = HallwaySceneComp;
			else
				ExitSocketCompList.Add(HallwaySceneComp);
		}

		// 브릿지 위치 조정
		if (EntranceSocketComp) {
			FTransform HallwayEntryTransform = EntranceSocketComp->GetComponentTransform();
			FTransform HallwayActorTransform = Hallway->GetActorTransform();
			FTransform ExitSocketRelativeTransform = HallwayEntryTransform.GetRelativeTransform(HallwayActorTransform);

			FTransform NewHallwayTransform = ExitSocketRelativeTransform.Inverse() * RoomSocketTransform;
			Hallway->SetActorTransform(NewHallwayTransform);
		}
		else
			PRINT_LOG(TEXT("My Log : %s "), TEXT("null ExitSocket"));
	}
	else
		PRINT_LOG(TEXT("My Log : %s "), TEXT("null HallwayActor"));
}
*/
