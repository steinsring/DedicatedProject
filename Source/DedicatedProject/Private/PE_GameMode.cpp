// Fill out your copyright notice in the Description page of Project Settings.


#include "PE_GameMode.h"
#include "PE_GameState.h"
#include "PE_PlayerController.h"
#include "PE_PlayerState.h"
#include "ProjectPlayer.h"
#include "PE_MapGenerator.h"
#include "UObject/ConstructorHelpers.h"

APE_GameMode::APE_GameMode() { //생성자
	GameStateClass = APE_GameState::StaticClass(); // 게임 스테이트
	PlayerStateClass = APE_PlayerState::StaticClass(); // 플레이어 스테이트
	PlayerControllerClass = APE_PlayerController::StaticClass(); // 플레이어 컨트롤러

	// 플레이어 캐릭터 블루프린트 가져오기
	static ConstructorHelpers::FClassFinder<APawn>PlayerPawnBPClass(TEXT("/Game/BluePrints/BP_ProjectPlayer"));
	if (PlayerPawnBPClass.Class != nullptr) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	if (HasAuthority()) // 서버에서만 실행!
	{
		UE_LOG(LogTemp, Warning, TEXT("서버에서 던전 생성 시작"));

		// 맵 생성기 생성
		//TObjectPtr<class APE_MapGenerator> MapGenerator = GetWorld()->SpawnActor<APE_MapGenerator>();
	}
}


