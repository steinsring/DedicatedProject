// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/PE_LobbyGameMode.h"
#include "Engine/World.h"
#include "Lobby/PE_LobbyPlayerState.h"
#include "Lobby/PE_LobbyPlayerController.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

APE_LobbyGameMode::APE_LobbyGameMode()
{
    PlayerStateClass = APE_LobbyPlayerState::StaticClass(); // 플레이어 스테이트
    PlayerControllerClass = APE_LobbyPlayerController::StaticClass(); // 플레이어 컨트롤러

	// 플레이어 캐릭터 블루프린트 가져오기
	static ConstructorHelpers::FClassFinder<APawn>PlayerPawnBPClass(TEXT("/Game/BluePrints/Lobby/BP_LobbyPawn.BP_LobbyPawn_C"));
	if (PlayerPawnBPClass.Class != nullptr) 
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void APE_LobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (APlayerState* PS = NewPlayer ? NewPlayer->PlayerState : nullptr)
    {
        // Steam 닉네임 얻기 시도
        FString Nickname = TEXT("");
        if (IOnlineSubsystem* OSS = IOnlineSubsystem::Get())
        {
            IOnlineIdentityPtr Identity = OSS->GetIdentityInterface();
            if (Identity.IsValid() && PS->GetUniqueId().IsValid())
            {
                const FUniqueNetId& NetId = *PS->GetUniqueId().GetUniqueNetId();
                Nickname = Identity->GetPlayerNickname(NetId);
            }
        }

        // 폴백: NetId 마지막 4자리 등
        if (Nickname.IsEmpty())
        {
            Nickname = FString::Printf(TEXT("Player_%d"), PS->GetPlayerId());
        }

        // 서버에서 확정 → 클라로 복제
        PS->SetPlayerName(Nickname);
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


