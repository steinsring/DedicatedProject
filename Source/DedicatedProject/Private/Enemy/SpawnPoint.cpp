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
}
