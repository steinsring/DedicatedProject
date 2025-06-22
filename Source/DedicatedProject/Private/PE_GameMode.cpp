// Fill out your copyright notice in the Description page of Project Settings.


#include "PE_GameMode.h"
#include "PE_GameState.h"
#include "PE_PlayerController.h"
#include "PE_PlayerState.h"
#include "ProjectPlayer.h"
#include "Map/PE_MapGenerator.h"
#include "UObject/ConstructorHelpers.h"

APE_GameMode::APE_GameMode() { //생성자
	GameStateClass = APE_GameState::StaticClass(); // 게임 스테이트
	PlayerStateClass = APE_PlayerState::StaticClass(); // 플레이어 스테이트
	PlayerControllerClass = APE_PlayerController::StaticClass(); // 플레이어 컨트롤러

	// 플레이어 캐릭터 블루프린트 가져오기
	static ConstructorHelpers::FClassFinder<APawn>PlayerPawnBPClass(TEXT("/Game/BluePrints/BP_ProjectPlayer.BP_ProjectPlayer_C"));
	if (PlayerPawnBPClass.Class != nullptr) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void APE_GameMode::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("서버에서 던전 생성 시작"));

		FActorSpawnParameters SpawnParams;
		GetWorld()->SpawnActor<APE_MapGenerator>(
			FVector::ZeroVector,             // 위치
			FRotator::ZeroRotator,          // 회전
			SpawnParams
		);
	}
}

// 이유는 모르겠는데 서버가 PostLogin을 스킵해서 강제로 실행하도록함. 이것을 하지 않으면 서버가 아닌 다른 클라이언트들은 character가 아닌 spectator로 생성이 되었음.
void APE_GameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    UE_LOG(LogTemp, Warning, TEXT("[GameMode] PostLogin called for %s"), *NewPlayer->GetName());

    if (NewPlayer->GetPawn())
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameMode] Player already has Pawn: %s"), *NewPlayer->GetPawn()->GetName());
    }
    else
    {
        AActor* StartSpot = ChoosePlayerStart(NewPlayer);
        APawn* Pawn = SpawnDefaultPawnFor(NewPlayer, StartSpot);
        if (!Pawn)
        {
            UE_LOG(LogTemp, Error, TEXT("❌ SpawnDefaultPawnFor FAILED for %s"), *NewPlayer->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("✅ Spawned Pawn: %s"), *Pawn->GetName());
            NewPlayer->Possess(Pawn);
        }
    }
}



