// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UI/PE_SessionItemRenderer.h"
#include "PE_FindSessions.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UPE_FindSessions : public UUserWidget
{
	GENERATED_BODY()
private:

protected:
	virtual void NativeConstruct() override;

	// 위젯에서 직접 바인딩
	UPROPERTY(meta = (BindWidget))
	UScrollBox* SessionsScrollBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SessionMessage;

	UPROPERTY(meta = (BindWidget))
	UButton* FindSessionsBtn;
	
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UPE_SessionItemRenderer> WidgetClass;
	
public:
	UFUNCTION()
	void OnFindSessionsClicked(); // 버튼 클릭 시 바인딩될 함수

	void AddItemRenderer(const FOnlineSessionSearchResult& SearchResult);
	void SetMessageText(const FString& Message);
	void EnableSearchButton();
};
