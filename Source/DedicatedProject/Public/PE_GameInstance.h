// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "UI/PE_FindSessions.h"
#include "PE_GameInstance.generated.h"

UCLASS()
class DEDICATEDPROJECT_API UPE_GameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
    IOnlineSessionPtr SessionInterface;                                                         // 온라인 session을 만들고 관리
    TSharedPtr<FOnlineSessionSettings> SessionSettings;                                         // 참가 가능 인수, LAN, Lobby사용 등 세션의 설정값을 저장
    TSharedPtr<FOnlineSessionSearch> SessionSearch;

    UPROPERTY()
    UPE_FindSessions* FindSessionsWidget;

    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);                       // Session 생성 종료 콜백 함수. 성공하면 레벨을 열고, 실패하면 메시지 출력
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnFindSessionsSuccess();
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

    // 스테이지 관리  ---------------------------------------------------------------
    UPROPERTY(VisibleAnywhere, Category = "LevelTravel")
    int32 LevelTravelCount = 1;

    UPROPERTY(EditAnywhere, Category = "LevelTravel")
    int32 MaxTravelBeforeLobby = 3;

	
public:
    virtual void Init() override;                                                               // 초기화, session을 위한 Online Subsystem 준비

    UFUNCTION(BlueprintCallable)                                                                // 블루프린트에서 호출 가능
    void CreateSession(const int32 MaxPlayers, const bool bIsLAN, const bool bIsLobby);         // Session 생성 함수

    UFUNCTION(BlueprintCallable)
    void FindSessions(int32 MaxResults, bool bIsLAN, bool bUseLobbies);

    void SetFindSessionsWidget(UPE_FindSessions* InWidget);
    
    void JoinFoundSession(const FOnlineSessionSearchResult& Result);                            // Session 참가 함수


    // 스테이지 관리  ---------------------------------------------------------------
    void NotifyLevelTravelTriggered(int32 Stage);
    void ResetLevelTravelCount();
    int32 GetCurrentLevelCount() const { return LevelTravelCount; }
    int32 GetMaxTravelLevelCount() const { return MaxTravelBeforeLobby; }
    bool IsGameEnd() const { return LevelTravelCount <= MaxTravelBeforeLobby; }
};  
