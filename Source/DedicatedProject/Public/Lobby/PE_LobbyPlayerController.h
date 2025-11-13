// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PE_LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API APE_LobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APE_LobbyPlayerController();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	
private:
	// Lobby UI ----------------------------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TSubclassOf<class UPE_LobbyUI> LobbyWidgetBPClass;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UPE_LobbyUI> LobbyWidget;								// 실제 생성된 인벤토리 위젯 인스턴스

	// 플레이어 리스트 ----------------------------------------------------------------------------------
protected:
	// PlayerList를 ReplicateUsing으로 선언
	UPROPERTY(ReplicatedUsing = OnRep_PlayerList)
	TArray<FString> PlayerList;

	UFUNCTION()
	void OnRep_PlayerList();   // 클라이언트에서만 실행될 함수

public:
	void UpdatePlayerList(const TArray<FString> DefaultPlayerList);

private:
	UPROPERTY()
	TSubclassOf<class UPE_LobbyPlayerName> PlayerNameWidgetBPClass;		// 플레이어 이름 위젯 클래스
	
};
