// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/PE_SessionItemRenderer.h"
#include "PE_GameInstance.h"

void UPE_SessionItemRenderer::NativeConstruct()
{
	Super::NativeConstruct();

	if (JoinBtn)
	{
		JoinBtn->OnClicked.AddDynamic(this, &UPE_SessionItemRenderer::OnJoinSessionClicked);
	}
}

void UPE_SessionItemRenderer::SetSession(const FOnlineSessionSearchResult& Result)
{
	CachedResult = Result;
	// 1. 서버 이름 추출
	FString ServerName;
	if (Result.Session.SessionSettings.Get(FName("SERVER_NAME_KEY"), ServerName))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("서버 이름: %s"), *ServerName));
		if (ServerNameLabel)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("서버 이름 라벨 설정 성공"));
			ServerNameLabel->SetText(FText::FromString(ServerName));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Session KEY 검색 안됨")));
		if (ServerNameLabel)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Unnamed Server 텍스트 설정 성공"));
			ServerNameLabel->SetText(FText::FromString(TEXT("Unnamed Server")));
		}
	}

	// 2. 현재 인원 / 최대 인원 텍스트 구성
	int32 NumOpenSlots = Result.Session.NumOpenPublicConnections;
	int32 MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
	int32 CurrentPlayers = MaxPlayers - NumOpenSlots;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("NumOpenSlots : %d"), NumOpenSlots));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("MaxPlayers : %d"), MaxPlayers));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("CurrentPlayers : %d"), CurrentPlayers));

	if (NumPlayersLabel)
	{
		FString PlayerCountString = FString::Printf(TEXT("%d / %d"), CurrentPlayers, MaxPlayers);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("인원 표시: %s"), *PlayerCountString));
		NumPlayersLabel->SetText(FText::FromString(PlayerCountString));
	}
}


void UPE_SessionItemRenderer::OnJoinSessionClicked()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UPE_GameInstance* PEGI = Cast<UPE_GameInstance>(GI))
		{
			PEGI->JoinFoundSession(CachedResult);
		}
	}
}



