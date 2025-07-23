// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PE_BasePickup.h"
#include "DedicatedProject.h"
#include "Player/ProjectPlayer.h"

void APE_BasePickup::Interact(AActor* Interactor)
{
	//PRINT_LOG(TEXT("Item : Test"));
	if (HasAuthority()) // 서버가 아니면 서버에게 처리 위임
	{
		PRINT_LOG(TEXT("Item : Server"));
		ServerInteract(Interactor);
		return;
	}
}

void APE_BasePickup::ServerInteract_Implementation(AActor* Interactor)
{
	// 플레이어가 E 키를 눌렀을 때 픽업
	if (AProjectPlayer* Player = Cast<AProjectPlayer>(Interactor))
	{
		PRINT_LOG(TEXT("Item : Pickup"));
		Player->AddItemToInventory(PickupID);
		Destroy();
		PRINT_LOG(TEXT("Item : Destroy"));
	}
}

