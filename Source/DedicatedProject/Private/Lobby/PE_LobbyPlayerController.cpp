// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/PE_LobbyPlayerController.h"
#include "DedicatedProject.h"
#include "Lobby/PE_LobbyUI.h"
#include "Lobby/PE_LobbyPlayerName.h"
#include "Net/UnrealNetwork.h"			//DOREPLIFETIME_CONDITION

APE_LobbyPlayerController::APE_LobbyPlayerController()
{
	// Lobby UI 로드 ----------------------------------------------------------------------
	static ConstructorHelpers::FClassFinder<UPE_LobbyUI> WidgetBPClass(TEXT("/Game/BluePrints/Lobby/WB_LobbyUI.WB_LobbyUI_C"));
	if (!WidgetBPClass.Succeeded())
	{
		PRINT_ERROR_LOG(TEXT("InventoryWidgetBPClass is NULL"));
		return;
	}

	LobbyWidgetBPClass = WidgetBPClass.Class;

	// PlayerName UI 로드 ----------------------------------------------------------------------
	static ConstructorHelpers::FClassFinder<UPE_LobbyPlayerName> WidgetBP1Class(TEXT("/Game/BluePrints/Lobby/WB_PlayerName.WB_PlayerName_C"));
	if (!WidgetBP1Class.Succeeded())
	{
		PRINT_ERROR_LOG(TEXT("PlayerNameWidget UI is NULL"));
		return;
	}

	PlayerNameWidgetBPClass = WidgetBP1Class.Class;
}

void APE_LobbyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APE_LobbyPlayerController, PlayerList);
}

void APE_LobbyPlayerController::BeginPlay()
{
	if (!IsLocalController()) return;

	// Inventory UI 생성 ----------------------------------------------------------------------
	LobbyWidget = CreateWidget<UPE_LobbyUI>(this, LobbyWidgetBPClass);
	if (LobbyWidget)
	{
		LobbyWidget->AddToViewport();
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("LobbyWidget Create Fail"));
	}
}

void APE_LobbyPlayerController::UpdatePlayerList(const TArray<FString> DefaultPlayerList)
{
	check(HasAuthority());	//서버 전용

	// 인벤토리 데이터 초기화 ----------------------------------------
	PlayerList.Reset();
	PlayerList = DefaultPlayerList;
	if (IsLocalController())
	{
		OnRep_PlayerList();
	}
}

void APE_LobbyPlayerController::OnRep_PlayerList()
{
	if (!IsLocalController()) return;
	if (!LobbyWidget) return;

	LobbyWidget->ClearLobbyUI();

	for (FString PlayerName : PlayerList)
	{
		// Inventory UI 생성 ----------------------------------------------------------------------
		UPE_LobbyPlayerName* PlayerNameWidget = CreateWidget<UPE_LobbyPlayerName>(this, PlayerNameWidgetBPClass);
		if (PlayerNameWidget)
		{
			PlayerNameWidget->SetNickName(PlayerName);
			LobbyWidget->AddPlayerName(PlayerNameWidget);
			//PlayerNameWidget->AddToViewport();
		}
		else
		{
			PRINT_ERROR_LOG(TEXT("PlayerNameWidget Create Fail"));
		}
	}
}


