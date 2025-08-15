// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/PE_InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "DedicatedProject.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Inventory/PE_InventoryItemBase.h"
#include "UI/PE_Inventory.h"

// Sets default values for this component's properties
UPE_InventoryComponent::UPE_InventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/DataTable/DT_ItemDataTable.DT_ItemDataTable"));

	if (ItemDataTable)									//데이터 테이블이 참조되었는지 확인
	{
		ItemDataTable->GetAllRows<FPE_ItemDataTable>(TEXT("ProjectPlayer"), ItemDataRows); //테이블의 모든 행을 지역배열로 가져온다.
	}
	else
	{
		PRINT_LOG(TEXT("ItemDataTable is NULL"));
	}
	
	static ConstructorHelpers::FClassFinder<UPE_Inventory> WidgetBPClass(TEXT("/Game/BluePrints/UI/WB_Inventory.WB_Inventory_C"));
	if (WidgetBPClass.Succeeded())
	{
		InventoryWidgetClass = WidgetBPClass.Class;
	}
	else
	{
		PRINT_LOG(TEXT("WidgetBPClass is NULL"));
	}

	// 입력
	// IMC 로드
	static const TCHAR* IMCPath = TEXT("/Game/Inputs/IMC_Inventory.IMC_Inventory");
	if (UInputMappingContext* IMC = LoadObject<UInputMappingContext>(nullptr, IMCPath))
	{
		InventoryMappingContext = IMC;
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("InventoryMappingContext is NULL"));
	}

	// IA 로드
	static const TCHAR* IAPaths[NumItemSlots] = {
		TEXT("/Game/Inputs/IA_ItemSlot1.IA_ItemSlot1"),
		TEXT("/Game/Inputs/IA_ItemSlot2.IA_ItemSlot2"),
		TEXT("/Game/Inputs/IA_ItemSlot3.IA_ItemSlot3"),
		TEXT("/Game/Inputs/IA_ItemSlot4.IA_ItemSlot4"),
		TEXT("/Game/Inputs/IA_ItemSlot5.IA_ItemSlot5"),
		TEXT("/Game/Inputs/IA_ItemSlot6.IA_ItemSlot6")
	};

	for (int32 i = 0; i < NumItemSlots; ++i)
	{
		if (UInputAction* IA = LoadObject<UInputAction>(nullptr, IAPaths[i]))
		{
			ItemSlotActions[i] = IA;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("IA_ItemSlot%d is NULL"), i + 1);
		}
	}
}


// Called when the game starts
void UPE_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!InventoryWidgetClass) 
	{
		return;
	}

	// 소유자에서 컨트롤러 가져오기
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}
	PRINT_LOG(TEXT("Is Begin twice?"));
	// 위젯 생성
	InventoryWidget = CreateWidget<UPE_Inventory>(PlayerController, InventoryWidgetClass);
	if (InventoryWidget)
	{
		InventoryWidget->AddToViewport();
	}

	if (PlayerController)
	{// 컴포넌트의 owner가 ProjectPlayer클래스인지 확인
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{// 해당 캐릭터의 컨트롤러를 가져와서
			Subsystem->AddMappingContext(InventoryMappingContext, 1);	// 매핑 컨텍스트 초기화
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{// 마찬가지로 액션 초기화
			for (TObjectPtr<UInputAction> ItemSlotAction : ItemSlotActions)
			{
				EnhancedInputComponent->BindAction(ItemSlotAction, ETriggerEvent::Triggered, this, &UPE_InventoryComponent::ItemSlotSelect);
			}
		}
	}
}

void UPE_InventoryComponent::ItemSlotSelect(const FInputActionInstance& Instance)
{
	const UInputAction* TriggeredAction = Instance.GetSourceAction();

	for (int32 i = 0; i < NumItemSlots; ++i)
	{
		if (ItemSlotActions[i] == TriggeredAction)
		{
			// 슬롯 번호 i 전달
			InventoryWidget->ItemSlotSelect(i);
			break;
		}
	}
}

// Called every frame
void UPE_InventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPE_InventoryComponent::FindItemData(FName ItemID)
{
	SearchedItemData = nullptr;						// 이전결과 초기화

	for (FPE_ItemDataTable* Row : ItemDataRows)		// ItemDataTable에서 아이템 정보 검색
	{
		if (Row->ItemID == ItemID)
		{
			SearchedItemData = Row;
			PRINT_LOG(TEXT("Find Item Success"));
			return;
		}
	}
	PRINT_LOG(TEXT("Find Item Fail"));
}


bool UPE_InventoryComponent::AddItem(FName ItemID)
{
	FindItemData(ItemID);
	InventoryWidget->AddItemToInventory(SearchedItemData->Icon, SearchedItemData->Quantity);
	return true;
}

bool UPE_InventoryComponent::RemoveItem(UPE_InventoryItemBase* Item)
{
	return true;
}
