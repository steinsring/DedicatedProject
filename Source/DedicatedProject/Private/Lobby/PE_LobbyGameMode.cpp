// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/PE_LobbyGameMode.h"
#include "PE_GameState.h"
#include "Player/PE_PlayerController.h"
#include "Player/PE_PlayerState.h"
#include "Engine/World.h"

APE_LobbyGameMode::APE_LobbyGameMode()
{
	GameStateClass = APE_GameState::StaticClass(); // 게임 스테이트
	PlayerStateClass = APE_PlayerState::StaticClass(); // 플레이어 스테이트
	PlayerControllerClass = APE_PlayerController::StaticClass(); // 플레이어 컨트롤러

	// 플레이어 캐릭터 블루프린트 가져오기
	static ConstructorHelpers::FClassFinder<APawn>PlayerPawnBPClass(TEXT("/Game/BluePrints/Lobby/BP_LobbyPawn.BP_LobbyPawn_C"));
	if (PlayerPawnBPClass.Class != nullptr) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}


void APE_LobbyGameMode::StartGameTravel()
{
	UWorld* World = GetWorld();
	if (World->GetAuthGameMode() == nullptr) return;

	// 게임 맵 이름 (패키징 시 프로젝트에 포함되어 있어야 함)
	static const TCHAR* TargetMap = TEXT("/Game/Maps/InGameMap1"); // 본인 경로로 교체

	// 모두 이동: 서버가 listen으로 열고 클라가 자동 따라옴
	World->ServerTravel(FString::Printf(TEXT("%s?listen"), TargetMap));
}


