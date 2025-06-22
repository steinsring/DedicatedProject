// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PE_FindSessions.h"
#include "DedicatedProject.h"
#include "PE_GameInstance.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "UI/PE_SessionItemRenderer.h"

void UPE_FindSessions::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UPE_GameInstance* PEGameInstance = Cast<UPE_GameInstance>(GameInstance))
		{
			// 위젯 자기 자신을 GameInstance에 등록해두면, 이후 C++에서 UI에 접근 가능
			PEGameInstance->SetFindSessionsWidget(this);
		}
	}
	// 버튼 클릭 시 C++ 함수 연결
	if (FindSessionsBtn)
	{
		FindSessionsBtn->OnClicked.AddDynamic(this, &UPE_FindSessions::OnFindSessionsClicked);
	}
}

void UPE_FindSessions::AddItemRenderer(const FOnlineSessionSearchResult& SearchResult)
{
	UWorld* World = GetWorld();
	if (!World || !SessionsScrollBox) return;

	APlayerController* LocalPlayerController = World->GetFirstPlayerController();
	if (!LocalPlayerController) return;

	UPE_SessionItemRenderer* Item = CreateWidget<UPE_SessionItemRenderer>(LocalPlayerController, WidgetClass);
	if (!Item) return;

	Item->SetSession(SearchResult);  // SetSession()은 Renderer1 위젯에서 구현해둬야 함
	SessionsScrollBox->AddChild(Item);
}

void UPE_FindSessions::SetMessageText(const FString& Message)
{
	if (SessionMessage)
	{
		SessionMessage->SetText(FText::FromString(Message));
	}
}

void UPE_FindSessions::EnableSearchButton()
{
	if (FindSessionsBtn)
	{
		FindSessionsBtn->SetIsEnabled(true);
	}
}

void UPE_FindSessions::OnFindSessionsClicked()
{
	// 1. UI 정리
	if (SessionsScrollBox)
	{
		SessionsScrollBox->ClearChildren();
	}

	// 2. 버튼 비활성화
	if (FindSessionsBtn)
	{
		FindSessionsBtn->SetIsEnabled(false);
	}

	// 3. 텍스트 메시지 출력
	if (SessionMessage)
	{
		SessionMessage->SetText(FText::FromString(TEXT("Searching Sessions...")));
	}

	// 4. GameInstance를 통해 실제 세션 검색 요청
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UPE_GameInstance* PEGameInstance = Cast<UPE_GameInstance>(GameInstance))
		{
			PEGameInstance->FindSessions(10, false, true); // 예: Max 10, LAN=off, Lobbies=on
		}
	}


}


