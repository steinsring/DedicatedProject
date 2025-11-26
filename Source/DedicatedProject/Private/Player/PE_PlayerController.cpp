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
#include "Player/PE_PlayerState.h"
#include "UI/PE_NotifyWindow.h"

#include "Kismet/GameplayStatics.h"

APE_PlayerController::APE_PlayerController()
{
	// Inventory UI 로드 ----------------------------------------------------------------------
	static ConstructorHelpers::FClassFinder<UPE_Inventory> WidgetBPClass(TEXT("/Game/BluePrints/UI/WB_Inventory.WB_Inventory_C"));
	if (WidgetBPClass.Succeeded())
	{
		InventoryWidgetBPClass = WidgetBPClass.Class;
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("InventoryWidgetBPClass is NULL"));
	}

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

	// 조준점
	static ConstructorHelpers::FClassFinder<UUserWidget> CrossHairBPClass(TEXT("/Game/BluePrints/UI/WB_CrossHair.WB_CrossHair_C"));
	if (CrossHairBPClass.Succeeded())
	{
		CrossHairWidgetBPClass = CrossHairBPClass.Class;
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("InventoryWidgetBPClass is NULL"));
	}

	// Notify UI 로드 ---------------------------------------------------------------------- 
	static ConstructorHelpers::FClassFinder<UPE_NotifyWindow> NotifyWidget(TEXT("/Game/BluePrints/UI/WB_NotifyWindow.WB_NotifyWindow_C")); 
	if (NotifyWidget.Succeeded()) 
	{ 
		NotifyWidgetClass = NotifyWidget.Class; //블루프린트에서 위젯 클래스를 불러온다. 
	} 
	else 
	{ 
		PRINT_ERROR_LOG(TEXT("NotifyWidgetClass is NULL")); 
	}

	// BGM setting----------------------------------------------------------------------------
	static ConstructorHelpers::FObjectFinder<USoundBase> BGM1_Obj(TEXT("/Game/Asset/BGM1.BGM1"));
	if (BGM1_Obj.Succeeded())
	{
		BGM1 = BGM1_Obj.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> BGM2_Obj(TEXT("/Game/Asset/BGM2.BGM2"));
	if (BGM2_Obj.Succeeded())
	{
		BGM2 = BGM2_Obj.Object;
	}
}

void APE_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;
	//PRINT_LOG(TEXT("BeginPlay: %s, IsLocal=%d"), *GetName(), IsLocalController());

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

	if (APE_PlayerState* PS = GetPlayerState<APE_PlayerState>())
	{
		PS->OnRep_InventoryData();
	}

	// 체력 --------------------------------------------------------------------------
	if (!HPBarWidget && HPBarWidgetClass)
	{
		HPBarWidget = CreateWidget<UPE_HPBarWidget>(this, HPBarWidgetClass);
		if (HPBarWidget)
		{
			PRINT_LOG(TEXT("HPBarWidget created in PC: %p"), HPBarWidget.Get());
			HPBarWidget->AddToViewport();
			BindHPBarToPawn(GetPawn());
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

	// 조준점---------------------------------------------------
	CrossHairWidget = CreateWidget<UUserWidget>(this, CrossHairWidgetBPClass);
	if (CrossHairWidget)
	{
		CrossHairWidget->AddToViewport();
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("CrossHairWidget Create Fail"));
	}

	SetRandomBGM();
	if (SelectedBGM)
	{
		UGameplayStatics::PlaySound2D(this, SelectedBGM);
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

void APE_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//if (!IsLocalController()) return;

	BindHPBarToPawn(InPawn);
}

void APE_PlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	if (!IsLocalController()) return;


	BindHPBarToPawn(GetPawn());
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
		//CurrentPlayer = GS->AlivePlayers[0]; // 현재 뷰 타겟이 플레이어가 아닌 경우 첫 번째 생존자 플레이어로 설정
		PRINT_LOG(TEXT("Current AlivePlayers: %d"), GS->AlivePlayers.Num());
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

void APE_PlayerController::BindHPBarToPawn(APawn* InPawn)
{
	if (!HPBarWidget || !InPawn) return;

	if (UHealthComponent* HealthComp = InPawn->FindComponentByClass<UHealthComponent>())
	{
		HPBarWidget->BindToHealthComponent(HealthComp);
	}
}

void APE_PlayerController::CreateNotify()
{
	// NotifyWidget UI 생성 ----------------------------------------------------------------------
	UPE_NotifyWindow* NotifyWidget = CreateWidget<UPE_NotifyWindow>(this, NotifyWidgetClass);
	if (!NotifyWidget)
	{
		PRINT_LOG(TEXT("NotifyWidget is Not Created!"));
		return;
	}

	NotifyWidget->AddToViewport();

	InventoryWidget->CreateNotify(NotifyWidget);
}

void APE_PlayerController::SetRandomBGM()
{
	const bool RandBool = FMath::RandBool();
	SelectedBGM = RandBool ? BGM1 : BGM2;
}
