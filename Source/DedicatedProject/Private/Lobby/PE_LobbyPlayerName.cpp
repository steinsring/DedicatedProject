// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/PE_LobbyPlayerName.h"
#include "Components/TextBlock.h"

void UPE_LobbyPlayerName::SetNickName(FString NickName)
{
	PlayerName->SetText(FText::FromString(NickName));
}

void UPE_LobbyPlayerName::SetReady(bool IsReady)
{
	if (IsReady)
	{
		ReadyText->SetText(FText::FromString(TEXT("Ready")));
	}
	else 
	{
		ReadyText->SetText(FText::FromString(TEXT("Not Ready")));
	}
}


