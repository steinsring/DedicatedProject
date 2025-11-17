// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayManager/PE_LevelTravelManager.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "DedicatedProject.h"
#include "PE_GameInstance.h"
#include "Player/PE_PlayerState.h"
#include "Player/ProjectPlayer.h"


// Sets default values
APE_LevelTravelManager::APE_LevelTravelManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractRange = CreateDefaultSubobject<USphereComponent>(TEXT("InteractRange"));
	SetRootComponent(InteractRange);
	InteractRange->SetSphereRadius(200.f);

	InteractMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractMesh"));
	InteractMesh->SetupAttachment(InteractRange);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("/Game/Asset/StarterBundle/ModularSci_Comm/Meshes/SM_Terminal_A_UI.SM_Terminal_A_UI"));
	if (StaticMesh.Succeeded())
	{
		InteractMesh->SetStaticMesh(StaticMesh.Object);
	}

	bReplicates = true;
}

// Called when the game starts or when spawned
void APE_LevelTravelManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APE_LevelTravelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APE_LevelTravelManager::Interact(AActor* Interactor)
{
	AProjectPlayer* PlayerCharacter = Cast<AProjectPlayer>(Interactor);
	if (!PlayerCharacter)
	{
		PRINT_LOG(TEXT("LevelTravelManager::Interact - Interactor is not AProjectPlayer"));
		return;
	}

	if (HasAuthority())
	{
		// 서버에서 직접 실행
		LevelTravel();
	}
	else
	{
		// 클라이언트 → 서버 RPC
		PlayerCharacter->Server_RequestLevelTravel(this);
	}
}

void APE_LevelTravelManager::LevelTravel()
{
	if (bTravelInProgress) // 중복 호출 방지
	{
		PRINT_LOG(TEXT("Travel skipped: already in progress"));
		return;
	}
	bTravelInProgress = true;

	UWorld* World = GetWorld();
	if (World->GetAuthGameMode() == nullptr)
	{
		PRINT_ERROR_LOG(TEXT("Travel To Level : %s"), TEXT("World is Null"));
		bTravelInProgress = false;
		return;
	}

	UPE_GameInstance* GameInstance = GetGameInstance<UPE_GameInstance>();
	if (GameInstance == nullptr)
	{
		PRINT_ERROR_LOG(TEXT("Travel To Level : %s"), TEXT("GameInstance is Null"));
		bTravelInProgress = false;
		return;
	}

	// 현재 접속해 있는 모든 플레이어 데이터 저장----------------------------------------------
	GameInstance->ClearPlayerDataMap();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC || !PC->PlayerState) continue;

		APE_PlayerState* PS = Cast<APE_PlayerState>(PC->PlayerState);
		if (!PS) continue;

		TArray<FItemData> PlayerInventoryData = PS->GetInventoryData();
		int32 PlayerFuelData = PS->GetCurrentFuel();

		const FString Key = PS->GetPlayerName(); // 나중엔 UniqueNetId로 하면 더 안전
		GameInstance->SaveStagePlayerData(Key, PlayerInventoryData, PlayerFuelData);
	}

	// 스테이지 정보를 GameInstance에서 가져와서 세팅--------------------------------
	int32 CurrentStage = GameInstance->GetCurrentLevelCount();
	int32 MaxStage = GameInstance->GetMaxTravelLevelCount();
	int32 NextStage = CurrentStage + 1;
	GameInstance->NotifyLevelTravelTriggered(NextStage);
	Multicast_NotifyLevelTravelTriggered(NextStage);

	if (NextStage <= MaxStage)
	{
		PRINT_LOG(TEXT("Travel To Next Level : %s, Current Level : %d"), TEXT("Stage End"), CurrentStage);
		World->ServerTravel(FString::Printf(TEXT("%s?listen"), TEXT("/Game/Maps/InGameMap1")));
		//World->ServerTravel(TEXT("/Game/Maps/InGameMap1?listen"), true);
	}
	else 
	{
		PRINT_LOG(TEXT("Travel To Lobby : %s"), TEXT("Game End"));
		GameInstance->ResetLevelTravelCount();
		World->ServerTravel(FString::Printf(TEXT("%s?listen"), TEXT("/Game/Maps/Lobby")));
	}
}

void APE_LevelTravelManager::Multicast_NotifyLevelTravelTriggered_Implementation(int32 NextStage)
{
	if (UPE_GameInstance* GameInstance = GetGameInstance<UPE_GameInstance>())
	{
		GameInstance->NotifyLevelTravelTriggered(NextStage);
	}
}

