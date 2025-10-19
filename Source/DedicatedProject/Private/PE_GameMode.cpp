// Fill out your copyright notice in the Description page of Project Settings.


#include "PE_GameMode.h"
#include "PE_GameState.h"
#include "Player/PE_PlayerController.h"
#include "Player/PE_PlayerState.h"
#include "Player/ProjectPlayer.h"
#include "Map/PE_MapGenerator.h"
#include "UObject/ConstructorHelpers.h"
#include "GameplayManager/PE_LevelTravelManager.h"

APE_GameMode::APE_GameMode() { //생성자
	GameStateClass = APE_GameState::StaticClass(); // 게임 스테이트
	PlayerStateClass = APE_PlayerState::StaticClass(); // 플레이어 스테이트
	PlayerControllerClass = APE_PlayerController::StaticClass(); // 플레이어 컨트롤러

	// 플레이어 캐릭터 블루프린트 가져오기
	static ConstructorHelpers::FClassFinder<APawn>PlayerPawnBPClass(TEXT("/Game/BluePrints/Player/BP_ProjectPlayer.BP_ProjectPlayer_C"));
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

    FVector LevelTravelLocation(-300.f, 1082.f, -218.f);

    GetWorld()->SpawnActor<APE_LevelTravelManager>(
        TargetLocation + LevelTravelLocation,                // 위치
        FRotator::ZeroRotator              // 회전
    );


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

    PlacePawnIfReady(NewPlayer);

    // PlayerState 초기화--------------------------------------------------------------
	
    if (APE_PlayerState* PlayerState = NewPlayer->GetPlayerState<APE_PlayerState>())
    {
        AllPlayerState.Add(PlayerState);
        PlayerState->InitializeDefaultData(); // 이미 되어 있으면 내부에서 무시
    }
}

void APE_GameMode::SetPlayerLocation(FVector PlayerStartLocation)
{
    // 1틱 뒤에 실행하여 Pawn이 완전히 Possess된 뒤 위치 이동
    GetWorld()->GetTimerManager().SetTimerForNextTick([this, PlayerStartLocation]()
        {
            for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
            {
                APlayerController* PC = It->Get();
                if (PC && PC->GetPawn())
                {
                    FVector NewLocation = PlayerStartLocation; // 커스텀 위치
                    PC->GetPawn()->SetActorLocation(PlayerStartLocation);
                }
            }
        });
}


void APE_GameMode::PlacePawnIfReady(APlayerController* PC)
{
    if (!bHasTarget || !PC) return;

    // Pawn이 아직 없으면, 다음 Possess 때 다시 시도
    if (APawn* P = PC->GetPawn())
    {
        // 이동 복제: Pawn은 기본적으로 ReplicateMovement 켜두세요
        // Character라면 TeleportTo 권장(스윕 OFF, 텔레포트 ON)
        FVector PlayerLocation(-100.f, 1082.f, -100.f);

        P->TeleportTo(TargetLocation + PlayerLocation, P->GetActorRotation(), /*bIsATest=*/false, /*bNoCheck=*/true);

        // (선택) 클라 보정이 필요하면:
        PC->ClientSetLocation(TargetLocation + PlayerLocation, PC->GetControlRotation());
    }
}

