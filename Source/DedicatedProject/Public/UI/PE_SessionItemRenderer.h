// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "PE_SessionItemRenderer.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API UPE_SessionItemRenderer : public UUserWidget
{
	GENERATED_BODY()
private:
	FOnlineSessionSearchResult CachedResult;

protected:
	virtual void NativeConstruct() override;

	// 블루프린트에 있는 바인딩용 텍스트 위젯
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UButton* JoinBtn;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* ServerNameLabel;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* NumPlayersLabel;

public:
	void SetSession(const FOnlineSessionSearchResult& Result);

	UFUNCTION()
	void OnJoinSessionClicked();
};
