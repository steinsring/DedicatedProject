// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/PE_AIController.h"
#include "GameFramework/Character.h" 

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"

const FName APE_AIController::HomePosKey(TEXT("HomePos"));
const FName APE_AIController::PatrolPosKey(TEXT("PatrolPos"));
const FName APE_AIController::TargetKey(TEXT("Target"));

APE_AIController::APE_AIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/AI/BB_PECharacter.BB_PECharacter"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/AI/BT_PECharacter.BT_PECharacter"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}

	BBComp = GetBlackboardComponent();
}

void APE_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	CollectWayPointsInCurrentNavVolume();
	UE_LOG(LogTemp, Warning, TEXT("[OnPossess] %s possessed %s"), *GetName(), *InPawn->GetName());
	
	if (UseBlackboard(BBAsset, BBComp))
	{
		//AIController가 Pawn을 소유했을때, HomePos의 위치를 Pawn의 위치로 세팅해준다.
		BBComp->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
		if (!RunBehaviorTree(BTAsset))
		{
			//
		}
	}
}

void APE_AIController::CollectWayPointsInCurrentNavVolume()
{
	UE_LOG(LogTemp, Log, TEXT("StartCollectWayPoints"));
	WayPoints.Empty();

	auto ControllingPawn = GetCharacter();
	if (nullptr == ControllingPawn) return;

	const FVector PawnLocation = ControllingPawn->GetActorLocation();
	// 1. NavMeshBoundsVolume 중 현재 캐릭터가 들어 있는 볼륨 찾기
	TArray<AActor*> NavVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANavMeshBoundsVolume::StaticClass(), NavVolumes);

	ANavMeshBoundsVolume* CurrentVolume = nullptr;
	for (auto VolumeActor : NavVolumes)
	{
		ANavMeshBoundsVolume* Volume = Cast<ANavMeshBoundsVolume>(VolumeActor);
		if (nullptr == Volume) continue;

		FVector Origin, Extent;
		Volume->GetActorBounds(false, Origin, Extent);

		const FVector Delta = (PawnLocation - Origin).GetAbs();
		if (Delta.X <= Extent.X && Delta.Y <= Extent.Y && Delta.Z <= Extent.Z)
		{
			CurrentVolume = Volume;
			UE_LOG(LogTemp, Log, TEXT("VolumeSelected"));
			break;
		}
	}

	if (nullptr == CurrentVolume) return;

	// 2. 현재 NavMeshBoundsVolume 안에 있는 Waypoint 찾기
	FVector VolumeOrigin, VolumeExtent;
	CurrentVolume->GetActorBounds(false, VolumeOrigin, VolumeExtent);

	TArray<AActor*> AllWaypoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), AllWaypoints);
	UE_LOG(LogTemp, Log, TEXT("Num Of Waypoints : %d"), AllWaypoints.Num());

	for (AActor* Actor : AllWaypoints)
	{
		ATargetPoint* Waypoint = Cast<ATargetPoint>(Actor);
		if (nullptr == Waypoint) continue;

		FVector Loc = Waypoint->GetActorLocation();
		FVector Delta = (Loc - VolumeOrigin).GetAbs();

		if (Delta.X <= VolumeExtent.X && Delta.Y <= VolumeExtent.Y && Delta.Z <= VolumeExtent.Z)
		{
			WayPoints.Add(Waypoint);
		}
	}
}
