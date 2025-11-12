// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PE_PlayerController.h"
#include "DedicatedProject.h"
#include "Player/ProjectPlayer.h"
#include "PE_GameState.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"

#include "UI/PE_Inventory.h"
#include "Inventory/PE_InventoryComponent.h"
#include "Inventory/FItemData.h"
#include "UI/PE_HPBarWidget.h"
#include "HealthComponent.h"

APE_PlayerController::APE_PlayerController()
{
	// Inventory UI 로드 ----------------------------------------------------------------------
	static ConstructorHelpers::FClassFinder<UPE_Inventory> WidgetBPClass(TEXT("/Game/BluePrints/UI/WB_Inventory.WB_Inventory_C"));
	if (!WidgetBPClass.Succeeded())
	{
		PRINT_ERROR_LOG(TEXT("InventoryWidgetBPClass is NULL"));
		return;
	}

	InventoryWidgetBPClass = WidgetBPClass.Class;

	// HealthComp UI 로드 ----------------------------------------------------------------------
	static ConstructorHelpers::FClassFinder<UUserWidget> HPBarWidgetAsset(TEXT("WidgetBlueprint'/Game/BluePrints/UI/WB_HPBar.WB_HPBar_C'"));
	if (HPBarWidgetAsset.Succeeded())
	{
		HPBarWidgetClass = HPBarWidgetAsset.Class; //블루프린트에서 위젯 클래스를 불러온다.
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("HPBarWidgetAsset is NULL"));
	}

}

void APE_PlayerController::BeginPlay()
{
	if (!IsLocalController()) return;

	// Inventory UI 생성 ----------------------------------------------------------------------
	InventoryWidget = CreateWidget<UPE_Inventory>(this, InventoryWidgetBPClass);
	if (InventoryWidget)
	{
		InventoryWidget->AddToViewport();
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("InventoryWidget Create Fail"));
	}

	// 체력 --------------------------------------------------------------------------
	if (HPBarWidgetClass)
	{
		HPBarWidget = CreateWidget<UPE_HPBarWidget>(this, HPBarWidgetClass);
		if (HPBarWidget)
		{
			HPBarWidget->AddToViewport();
			HPBarWidget->BindToHealthComponent(GetPawn()->FindComponentByClass<UHealthComponent>());
		}
		else
		{
			PRINT_ERROR_LOG(TEXT("HPBarWidget is Not Created"));
		}
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("HPBarWidgetClass is NULL"));
	}
}

void APE_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInput) return;

	UInputMappingContext* IMC_Spectator = LoadObject<UInputMappingContext>
		(nullptr, TEXT("/Game/Inputs/IMC_ProjectPlayer.IMC_ProjectPlayer"));
	
	if (IMC_Spectator)
	{
		auto SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		if (SubSystem) SubSystem->AddMappingContext(IMC_Spectator, 0);
	}
	
	IA_Spectator = LoadObject<UInputAction>
		(nullptr, TEXT("/Game/Inputs/IA_Spectator.IA_Spectator"));
	if (IA_Spectator)
	{
		EnhancedInput->BindAction(IA_Spectator, ETriggerEvent::Started, this, &APE_PlayerController::OnNextSpectateTarget);
	}
}

void APE_PlayerController::ItemSlotSelect(int32 i)
{ // 선택 위젯 슬롯 변경
	InventoryWidget->ItemSlotSelect(i);
}

void APE_PlayerController::InventoryDataUpdate(const TArray<FItemData>& InventoryData)
{
	InventoryWidget->SetInventoryData(InventoryData);
}

void APE_PlayerController::OnNextSpectateTarget()
{
	PRINT_LOG(TEXT("Spectate Next Target"));
	if (!IsLocalController())
	{
		PRINT_LOG(TEXT("Not Local Controller"));
		return;
	}

	AProjectPlayer* PlayerPawn = Cast<AProjectPlayer>(GetPawn());
	if (PlayerPawn && !PlayerPawn->GetIsDead())
	{
		PRINT_LOG(TEXT("Player is Alive"));
		return; // 살아있는 플레이어는 스펙테이트 모드로 진입 불가
	}

	APE_GameState* GS = GetWorld()->GetGameState<APE_GameState>();
	if (!GS || GS->AlivePlayers.Num() == 0)
	{
		PRINT_LOG(TEXT("No Alive Players to Spectate"));
		return;
	}

	AActor* CurrentViewTarget = GetViewTarget();
	AProjectPlayer* CurrentPlayer = Cast<AProjectPlayer>(CurrentViewTarget);
	if (!CurrentPlayer)
	{
		PRINT_LOG(TEXT("CurrentViewTarget is not AProjectPlayer"));
		CurrentPlayer = GS->AlivePlayers[0]; // 현재 뷰 타겟이 플레이어가 아닌 경우 첫 번째 생존자 플레이어로 설정
	}
	AProjectPlayer* NextPlayer = GS->GetNextAlivePlayer(CurrentPlayer);

	if (NextPlayer)
	{
		PRINT_LOG(TEXT("Spectating Next Player: %s"), *NextPlayer->GetName());
		SetViewTargetWithBlend(NextPlayer, 0.0f);
	}
	else
	{
		PRINT_LOG(TEXT("NextPlayer is NULL"));
	}
}

