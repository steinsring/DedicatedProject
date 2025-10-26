// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PE_BasePickup.h"
#include "DedicatedProject.h"
#include "Player/ProjectPlayer.h"

void APE_BasePickup::Interact(AActor* Interactor)
{
	//ServerInteract(Interactor);	-> rpc는 owner가 있는 액터만
	// 플레이어가 E 키를 눌렀을 때 픽업
	if (AProjectPlayer* Player = Cast<AProjectPlayer>(Interactor))
	{
		PRINT_LOG(TEXT("Item : Pickup"));
		Player->AddItemToInventory(PickupID, PickupQuantity);
		PRINT_LOG(TEXT("Item : Destroy"));
	}
}

