// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/PE_LobbyUI.h"
#include "DedicatedProject.h"
#include "Components/VerticalBox.h"
#include "Lobby/PE_LobbyPlayerName.h"

void UPE_LobbyUI::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPE_LobbyUI::AddPlayerName(UPE_LobbyPlayerName* NickNameUI)
{
	PlayerListUI->AddChild(NickNameUI);
}

void UPE_LobbyUI::ClearLobbyUI()
{
	PlayerListUI->ClearChildren();
}


