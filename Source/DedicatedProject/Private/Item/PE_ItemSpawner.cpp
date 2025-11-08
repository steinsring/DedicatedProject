// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PE_ItemSpawner.h"
#include "Components/BoxComponent.h"
#include "Item/PE_ItemPickup.h"
#include "DedicatedProject.h"


// Sets default values
APE_ItemSpawner::APE_ItemSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 스폰할 아이템들
	static ConstructorHelpers::FClassFinder<APE_ItemPickup> Item1_BP(TEXT("/Game/BluePrints/WorldItem/BP_EMP_Granade_World"));
	static ConstructorHelpers::FClassFinder<APE_ItemPickup> Item2_BP(TEXT("/Game/BluePrints/WorldItem/BP_FlashLight"));
	static ConstructorHelpers::FClassFinder<APE_ItemPickup> Item3_BP(TEXT("/Game/BluePrints/WorldItem/BP_Fuel"));

	// 유효하면 배열에 추가
	if (Item1_BP.Succeeded())
	{
		SpawnableItems.Add(Item1_BP.Class);
	}

	if (Item2_BP.Succeeded())
	{
		SpawnableItems.Add(Item2_BP.Class);
	}

	if (Item3_BP.Succeeded())
	{
		SpawnableItems.Add(Item3_BP.Class);
	}

}

// Called when the game starts or when spawned
void APE_ItemSpawner::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APE_ItemSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APE_ItemSpawner::Spawn(const FVector Location, const FRotator Rotation)
{
	FActorSpawnParameters SpawnParams;
	// 다른 오브젝트와 충돌을 피함
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
	PRINT_LOG(TEXT("Spawn Test"));
	if (SpawnableItems.IsEmpty()) return;
	
	// 배열에서 아이템 선택
	TSubclassOf<APE_ItemPickup> Item = SpawnableItems[FMath::RandRange(0, SpawnableItems.Num() - 1)];

	APE_ItemPickup* SpawnedItem = GetWorld()->SpawnActor<APE_ItemPickup>(Item, Location, Rotation, SpawnParams);
	PRINT_LOG(TEXT("[ItemSpawner] Spawning item at Location: %s, Rotation: %s"),
		*Location.ToString(),
		*Rotation.ToString());
	if (!SpawnedItem) return;
	PRINT_LOG(TEXT("[ItemSpawner] Spawned: %s at %s"),
		*SpawnedItem->GetName(),
		*SpawnedItem->GetActorLocation().ToString());
}
