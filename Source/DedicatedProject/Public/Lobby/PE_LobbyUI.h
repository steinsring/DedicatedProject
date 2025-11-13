// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PE_LobbyUI.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UPE_LobbyUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UVerticalBox> PlayerListUI;				// 플레이어 리스트 컨테이너

public:
	void AddPlayerName(class UPE_LobbyPlayerName* NickNameUI);

	void ClearLobbyUI();
};
