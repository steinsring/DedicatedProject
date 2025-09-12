// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/WayPoint.h"
#include "DedicatedProject.h"

AWayPoint::AWayPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	AActor* ParentActor = this->GetAttachParentActor();
	if (this->GetAttachParentActor() && this->GetAttachParentActor()->IsA<ASpawnPoint>())
	{
		ParentSpawnPoint = Cast<ASpawnPoint>(this->GetAttachParentActor());
		PRINT_LOG(TEXT("My Log : %s"), *ParentSpawnPoint->GetName());
	}
	else
	{
		if (ParentActor != nullptr)
		{
			PRINT_LOG(TEXT("My Log : %s"), *ParentActor->GetName());
		}
		else
		{
			PRINT_LOG(TEXT("My Log : %s"), TEXT("Parent Actor is nullptr"));
		}
	}
}
