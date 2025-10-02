// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayManager/PE_LevelTravelManager.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PE_GameMode.h"


// Sets default values
APE_LevelTravelManager::APE_LevelTravelManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractRange = CreateDefaultSubobject<USphereComponent>(TEXT("InteractRange"));
	SetRootComponent(InteractRange);
	InteractRange->SetSphereRadius(200.f);

	InteractMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractMesh"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("/Game/Asset/StarterBundle/ModularSci_Comm/Meshes/SM_Terminal_A_UI.SM_Terminal_A_UI"));
	if (StaticMesh.Succeeded())
	{
		InteractMesh->SetStaticMesh(StaticMesh.Object);
	}

}

// Called when the game starts or when spawned
void APE_LevelTravelManager::BeginPlay()
{
	Super::BeginPlay();

	APE_GameMode* GameMode = Cast<APE_GameMode>(GetWorld()->GetAuthGameMode());
	SetActorLocation(GameMode->GetTargetLocation());
}

// Called every frame
void APE_LevelTravelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APE_LevelTravelManager::Interact(AActor* Interactor)
{
	UWorld* World = GetWorld();
	if (World->GetAuthGameMode() == nullptr) return;

	// 게임 맵 이름 (패키징 시 프로젝트에 포함되어 있어야 함)
	static const TCHAR* TargetMap = TEXT("/Game/Maps/InGameMap1"); // 본인 경로로 교체

	// 모두 이동: 서버가 listen으로 열고 클라가 자동 따라옴
	World->ServerTravel(FString::Printf(TEXT("%s?listen"), TargetMap));
}

