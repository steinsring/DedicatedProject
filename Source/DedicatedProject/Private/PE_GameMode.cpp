// Fill out your copyright notice in the Description page of Project Settings.


#include "PE_GameMode.h"
#include "PE_GameState.h"
#include "Player/PE_PlayerController.h"
#include "Player/PE_PlayerState.h"
#include "Player/ProjectPlayer.h"
#include "Common_AnimInstance.h"
#include "Map/PE_MapGenerator.h"
#include "UObject/ConstructorHelpers.h"
#include "GameplayManager/PE_LevelTravelManager.h"

#include "DedicatedProject.h"

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

    NewPlayer->GetOnNewPawnNotifier().AddLambda([this, NewPlayer](APawn* InPawn)
    {
        if (AProjectPlayer* ProjectPlayer = Cast<AProjectPlayer>(InPawn))
        {
            APE_GameState* GS = GetGameState<APE_GameState>();
            if (GS)
            {
                GS->AddAlivePlayer(ProjectPlayer);
                PRINT_LOG(TEXT("Added player to alive list in GameState. Total Alive Players: %d"), GS->AlivePlayers.Num());
            }
        }
    });
	
    if (NewPlayer->GetPawn())
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameMode] Player already has Pawn: %s"), *NewPlayer->GetPawn()->GetName());

        if (AProjectPlayer* ProjectPlayer = Cast<AProjectPlayer>(NewPlayer->GetPawn()))
        {
            APE_GameState* GS = GetGameState<APE_GameState>();
            if (GS)
            {
                GS->AddAlivePlayer(ProjectPlayer);
                PRINT_LOG(TEXT("Added player to alive list in GameState. Total Alive Players: %d"), GS->AlivePlayers.Num());
            }
        }
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

void APE_GameMode::HandlePlayerDeath(AController* DeadController)
{
	PRINT_LOG(TEXT("HandlePlayerDeath called"));
	if (!DeadController) return;
	APlayerController* DeadPC = Cast<APlayerController>(DeadController);
	if (!DeadPC) return;

	AProjectPlayer* DeadPlayer = Cast<AProjectPlayer>(DeadPC->GetPawn());
    if (DeadPlayer)
    {
		PRINT_LOG(TEXT("Processing death for player: %s"), *DeadPlayer->GetName());
		UCommon_AnimInstance* AnimInstance = Cast<UCommon_AnimInstance>(DeadPlayer->GetMesh()->GetAnimInstance());
        if (AnimInstance) AnimInstance->SetIsDead(true);
        

		

		// 게임 상태에서 생존자/사망자 목록 갱신
		APE_GameState* GS = GetGameState<APE_GameState>();
        if (!GS) return;

        // 관전 대상 찾기
		AProjectPlayer* NextTarget = GS ? GS->GetNextAlivePlayer(DeadPlayer) : nullptr;
        if (NextTarget)
        {
            //DeadPC->SetViewTargetWithBlend(NextTarget, 0.0f);
            PRINT_LOG(TEXT("Spectating Next Player: %s"), *NextTarget->GetName());
        }

        GS->RemoveAlivePlayer(DeadPlayer);
        GS->AddDeadPlayer(DeadPlayer);
        GS->DeadPlayerStates.Add(DeadPlayer->GetPlayerState<APlayerState>());

        DeadPlayer->DetachFromControllerPendingDestroy();
		
        PRINT_LOG(TEXT("Updated GameState: AlivePlayers=%d, DeadPlayers=%d"), GS->AlivePlayers.Num(), GS->DeadPlayers.Num());
    }
}

void APE_GameMode::HandlePlayerRespawn(APlayerState* DeadPS, AProjectPlayer* TargetDummy)
{
	if (!DeadPS || !TargetDummy) return;

	APlayerController* DeadPC = Cast<APlayerController>(DeadPS->GetOwner());
	if (!DeadPC) return;

	APE_GameState* GS = GetGameState<APE_GameState>();
	if (!GS) return;

	TargetDummy->SetPowerState_Multicast(true); // 활성화
	// 관전 해제
	DeadPC->SetViewTargetWithBlend(TargetDummy, 0.0f);
	// Pawn Possess
	DeadPC->Possess(TargetDummy);


	// 게임 상태에서 생존자/사망자 목록 갱신
	GS->RemoveDeadPlayer(Cast<AProjectPlayer>(TargetDummy));
	GS->AddAlivePlayer(Cast<AProjectPlayer>(TargetDummy));
	GS->DeadPlayerStates.Remove(DeadPS);
	PRINT_LOG(TEXT("Player Respawned: %s. Updated GameState: AlivePlayers=%d, DeadPlayers=%d"), *TargetDummy->GetName(), GS->AlivePlayers.Num(), GS->DeadPlayers.Num());
}