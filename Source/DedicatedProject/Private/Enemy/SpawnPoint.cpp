// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/SpawnPoint.h"
#include "DedicatedProject.h"

ASpawnPoint::ASpawnPoint()
{

}

void ASpawnPoint::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	WayPoints.Empty();

	TArray<UChildActorComponent*> ChildActorComps;
	GetComponents<UChildActorComponent>(ChildActorComps);

	for (auto* Comp : ChildActorComps)
	{
		if (ATargetPoint* TP = Cast<ATargetPoint>(Comp->GetChildActor()))
		{
			WayPoints.Add(TP);
		}
	}
	PRINT_LOG(TEXT("My Log : %s %d"), TEXT("TargetPoints Count : "), WayPoints.Num());
}
