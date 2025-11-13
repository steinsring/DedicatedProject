// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PE_LobbyPlayerName.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UPE_LobbyPlayerName : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ReadyText;

public:
	void SetNickName(FString NickName);
	void SetReady(bool IsReady);
};
