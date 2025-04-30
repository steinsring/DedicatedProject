// Fill out your copyright notice in the Description page of Project Settings.

#include "PE_MapGenerator.h"
#include "DedicatedProject.h"
#include "Components/BoxComponent.h"

// Sets default values
APE_MapGenerator::APE_MapGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 맵을 생성할 위치 설정
	GenerateArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Generate Area"));
	RootComponent = GenerateArea;
	GenerateArea->SetupAttachment(RootComponent);
	GenerateArea->SetBoxExtent(FVector(MapSizeX, MapSizeY, MapSizeZ));

	/*
	static ConstructorHelpers::FClassFinder<AActor>BPMapClass(TEXT("/Game/Asset/HomeMade/BP_Room1"));
	GeneratableMaps.Add(BPMapClass.Class);
	static ConstructorHelpers::FClassFinder<AActor>BPMapClass(TEXT("/Game/Asset/HomeMade/BP_ControlRoom1"));
	GeneratableMaps.Add(BPMapClass.Class);
	*/
	TArray<FString> BPRoomPaths = {
		TEXT("/Game/Asset/HomeMade/BP_RoomNode1.BP_RoomNode1_C")
	};

	for (const FString& Path : BPRoomPaths)
	{
		UClass* LoadedClass = LoadClass<AActor>(nullptr, *Path);
		if (LoadedClass)
			GeneratableMaps.Add(LoadedClass);
	}

	TArray<FString> BPBridgePaths = {
		TEXT("/Game/Asset/HomeMade/BP_HallwayConnectBridge1.BP_HallwayConnectBridge1_C"),
		TEXT("/Game/Asset/HomeMade/BP_Hallway1-2.BP_Hallway1-2_C"),
		TEXT("/Game/Asset/HomeMade/BP_Hallway1-3.BP_Hallway1-3_C"),
		TEXT("/Game/Asset/HomeMade/BP_Hallway1-4.BP_Hallway1-4_C")
	};

	for (const FString& Path : BPBridgePaths)
	{
		UClass* LoadedClass = LoadClass<AActor>(nullptr, *Path);
		if (LoadedClass)
			GeneratableBridges.Add(LoadedClass);
	}
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
{// 모든 leaf 수집
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

	for (TSharedPtr<FBSPNode> Leaf : LeavesList) {
		TSubclassOf<AActor> Map = GeneratableMaps[FMath::RandRange(0, GeneratableMaps.Num() - 1)]; // 저장된 배열안의 StaticMesh중 랜덤하게 하나 골라서
		// GenerateArea 영역 안의 노드 위치에 랜덤 방향으로 Spawn
		float RandomAngle = 90 * FMath::RandRange(0, 3);
		const FRotator Rotation = FRotator(0.0f, RandomAngle, 0.0f); //Z축(Yaw) 회전을 랜덤하게
		const FVector Center = FVector((Leaf->MaxCoordinate + Leaf->MinCoordinate) * 0.5f, 0.0f);
		const FVector Location = Center;
		//PRINT_LOG(TEXT("My Log : %s "), *Center.ToString());

		AActor* Room = GetWorld()->SpawnActor<AActor>(Map, Location, Rotation, SpawnParams); // Actor를 맵에 Spawn
		/*
		// 연결 브릿지 블루프린트 스폰
		if (Room) { 
			// 방의 루트 컴포넌트의 자식들에서 Socket만 분리
			TArray<USceneComponent*> RoomSceneCompList = FindSceneCompList(Room);

			// 방의 모든 소켓의 위치에 브릿지 스폰
			for (USceneComponent* Comp : RoomSceneCompList) { 
				if (Comp) {

					SpawnHallway(Comp, Leaf);
				}
				else
					PRINT_LOG(TEXT("My Log : %s "), TEXT("null SceneComponent"));
			}
		}
		else
			PRINT_LOG(TEXT("My Log : %s "), TEXT("null RoomActor"));
		*/
	}
	
}

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

