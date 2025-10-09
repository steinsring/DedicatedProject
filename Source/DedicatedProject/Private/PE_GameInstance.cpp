// Fill out your copyright notice in the Description page of Project Settings.


#include "PE_GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Online/OnlineSessionNames.h"
#include "Kismet/GameplayStatics.h"
#include "DedicatedProject.h"

void UPE_GameInstance::Init()
{
    Super::Init();

    // Online Subsystem 초기화
    
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();                                                                      // 현재 사용중인 OnlineSubsystem
    if (Subsystem)
    {   
        SessionInterface = Subsystem->GetSessionInterface();                                                                    // 현재 사용중인 interface -> 세션 생성, 세션 검색, 세션 참가

        // session 생성 종료시 콜백함수 호출
        if (SessionInterface.IsValid()) {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPE_GameInstance::OnCreateSessionComplete);    // CreateSession 델리게이트 바인딩
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPE_GameInstance::OnFindSessionsComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPE_GameInstance::OnJoinSessionComplete);        // JoinSession 델리게이트 바인딩
        }
            
    }
}

static FName MakePIESessionName(UWorld* World)
{// 에디터 PIE에서만 고유 세션명 사용 (단일 프로세스 PIE 콜백 섞임 방지)
#if WITH_EDITOR
    if (GEngine && World)
    {
        if (const FWorldContext* Ctx = GEngine->GetWorldContextFromWorld(World))
        {
            return FName(*FString::Printf(TEXT("GameSession_%d"), Ctx->PIEInstance));
        }
    }
#endif
    // 패키징/실게임에서는 공통 이름 사용
    return NAME_GameSession;
}

void UPE_GameInstance::CreateSession(const int32 MaxPlayers, const bool bIsLAN, const bool bIsLobby)
{
    // SessionInterface가 없는경우 종료
    if (!SessionInterface.IsValid()) return;

    // session 이름
    const FName SessionName = MakePIESessionName(GetWorld());                 

    // 같은 이름의 기존 세션이 있다면 제거
    auto ExistingSession = SessionInterface->GetNamedSession(SessionName);
    if (ExistingSession) 
    {
        SessionInterface->DestroySession(SessionName);
    }
    PRINT_LOG(TEXT("CreateSession: bIsLobby = %s"), bIsLobby ? TEXT("true") : TEXT("false"));
    // session 설정
    SessionSettings = MakeShareable(new FOnlineSessionSettings());          // session설정을 포인터로 관리
    SessionSettings->bIsLANMatch = bIsLAN;                                  // LAN 환경에서만 접속할 수 있도록 설정
    SessionSettings->NumPublicConnections = MaxPlayers;                     // 접속 가능한 최대 플레이어 수
    SessionSettings->bShouldAdvertise = true;                               // 세션을 목록에 보여줄지 여부
    SessionSettings->bUsesPresence = bIsLobby;                              // 친구 초대 등을 위한 존재 정보 사용
    SessionSettings->bAllowJoinInProgress = true;                           // 게임 도중에 접속 허용 여부
    SessionSettings->bUseLobbiesIfAvailable = bIsLobby;                     // 로비 시스템을 사용하는 경우
    SessionSettings->bAllowJoinViaPresence = true;
    SessionSettings->bIsDedicated = false;
    SessionSettings->Set(SEARCH_PRESENCE, bIsLobby, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    SessionSettings->Set(SEARCH_LOBBIES, bIsLobby, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    // CreateSession에서 Key-Value 설정
    SessionSettings->Set(FName("SERVER_NAME_KEY"), FString("ServerEntry1"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    // 실제 세션 생성
    const ULocalPlayer* LocalPlayer = GetFirstGamePlayer();                 // UGameViewportClient의 PlayerList[0] 참조, 현재 첫 번째 로컬 플레이어 객체를 반환
    if (LocalPlayer && LocalPlayer->GetPreferredUniqueNetId().IsValid())    // 세션 생성에 사용할 수 있는 유효한 FUniqueNetId를 반환하는 GetPreferredUniqueNetId
    {
        PRINT_LOG(TEXT("My Log : %s"), TEXT("Start to create session."));
        SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), SessionName, *SessionSettings);
    }
}

void UPE_GameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    // 1) 내 월드의 고유 세션명만 처리
    const FName MyName = MakePIESessionName(GetWorld());
    if (SessionName != MyName) return;

    if (bWasSuccessful)
    { //세션을 성공적으로 만들었다면
        PRINT_LOG(TEXT("My Log : %s"), TEXT("Session Created Success"));
        SessionInterface->StartSession(SessionName);
        UGameplayStatics::OpenLevel(GetWorld(), "/Game/Maps/Lobby", true, "listen");    // Lobby 맵으로 이동
    }
    else 
    {
        PRINT_ERROR_LOG(TEXT("My Log : %s"), TEXT("Failed to create session."));
    }
}

void UPE_GameInstance::FindSessions(int32 MaxResults, bool bIsLAN, bool bUseLobbies)
{
    if (!SessionInterface.IsValid())
    {
        PRINT_ERROR_LOG(TEXT("My Log : %s"), TEXT("세션 인터페이스가 유효하지 않습니다."));
        return;
    }
    PRINT_LOG(TEXT("FindSessions: bUseLobbies = %s"), bUseLobbies ? TEXT("true") : TEXT("false"));
    SessionSearch = MakeShareable(new FOnlineSessionSearch());                                                  // 객체를 새로 생성해서 검색 조건을 설정
    SessionSearch->MaxSearchResults = MaxResults;                                                               // 최대 검색 방 정보 개수
    SessionSearch->bIsLanQuery = bIsLAN;                                                                        // LAN 모드 여부
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bUseLobbies, EOnlineComparisonOp::Equals);
    SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, bUseLobbies, EOnlineComparisonOp::Equals);

    ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
    {
        PRINT_ERROR_LOG(TEXT("My Log : %s"), TEXT("로컬 플레이어 ID를 가져올 수 없습니다."));
        return;
    }

    // 세션 검색을 비동기적으로 시작
    bool bStarted = SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
    if (!bStarted)
    {
        PRINT_ERROR_LOG(TEXT("My Log : %s"), TEXT("세션 검색 시작에 실패했습니다"));
    }
}

void UPE_GameInstance::SetFindSessionsWidget(UPE_FindSessions* InWidget)
{
    FindSessionsWidget = InWidget;
}

void UPE_GameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (!SessionSearch.IsValid())
    {
        PRINT_ERROR_LOG(TEXT("My Log : %s"), TEXT("세션 검색 데이터가 유효하지 않습니다."));
        return;
    }

    if (bWasSuccessful)
    {
        PRINT_LOG(TEXT("My Log : %s"), TEXT("세션 검색 성공"));
        FindSessionsWidget->SetMessageText("searching sessions success");
        OnFindSessionsSuccess();
    }
    else
    {
        PRINT_ERROR_LOG(TEXT("My Log : %s"), TEXT("세션 검색 실패"));
        FindSessionsWidget->SetMessageText("Error searching for available sessions");
        FindSessionsWidget->EnableSearchButton();
    }
}

void UPE_GameInstance::OnFindSessionsSuccess()
{
    TArray<FOnlineSessionSearchResult> SearchResult = SessionSearch->SearchResults;
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("OnFindSessionsSuccess SearchResult : %d"), SearchResult.Num()));
    if (SearchResult.Num() == 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("세션 검색 결과가 없음.")));
        PRINT_ERROR_LOG(TEXT("My Log : %s"), TEXT("세션 검색 결과가 없음."));
    }
    for (const FOnlineSessionSearchResult& Session : SearchResult) 
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("세션 렌더링 시도")));
        PRINT_ERROR_LOG(TEXT("My Log : %s"), TEXT("세션 렌더링 시도"));
        FindSessionsWidget->AddItemRenderer(Session);
    }
    FindSessionsWidget->EnableSearchButton();
}

void UPE_GameInstance::JoinFoundSession(const FOnlineSessionSearchResult& Result)
{
    if (!SessionInterface.IsValid())
    {
        PRINT_ERROR_LOG(TEXT("SessionInterface is invalid"));
        return;
    }

    ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
    if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
    {
        PRINT_ERROR_LOG(TEXT("Invalid LocalPlayer for JoinSession"));
        return;
    }

    // ✅ Unreal Engine 5.5 Workaround: 강제 세션 설정 일치
    FOnlineSessionSearchResult& ModifiableResult = const_cast<FOnlineSessionSearchResult&>(Result);
    ModifiableResult.Session.SessionSettings.bUsesPresence = true;
    ModifiableResult.Session.SessionSettings.bUseLobbiesIfAvailable = true;

    SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
}

void UPE_GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    UE_LOG(LogTemp, Warning, TEXT("JoinSessionComplete: SessionName = %s"), *SessionName.ToString());
    FName ExpectedSessionName = NAME_GameSession;
    UE_LOG(LogTemp, Warning, TEXT("Expected SessionName = %s"), *ExpectedSessionName.ToString());

    if (!SessionInterface.IsValid()) return;

    FString Address;
    if (SessionInterface->GetResolvedConnectString(SessionName, Address))
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            PC->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GetResolvedConnectString failed for SessionName: %s"), *SessionName.ToString());
        PRINT_ERROR_LOG(TEXT("Failed to resolve connect string"));
    }
}

// 스테이지 관리 -------------------------------------------------------------------------
void UPE_GameInstance::NotifyLevelTravelTriggered(int32 Stage)
{
    LevelTravelCount = Stage;
}

// 초기화용
void UPE_GameInstance::ResetLevelTravelCount()
{
    LevelTravelCount = 1;
}



