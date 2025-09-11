// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/PE_InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "DedicatedProject.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Inventory/PE_InventoryItemBase.h"
#include "Player/PE_ItemThrowableComponent.h"
#include "UI/PE_Inventory.h"

// Sets default values for this component's properties
UPE_InventoryComponent::UPE_InventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
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

void UPE_InventoryComponent::SetComponent(TObjectPtr<class UPE_ItemThrowableComponent> Component)
{
	ItemThrowableComponent = Component;
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
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PlayerController)
	{
		return;
	}
	//PRINT_LOG(TEXT("Is Begin twice?"));
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
			SelectedSlotNumber = i;
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

bool UPE_InventoryComponent::AddItem(FName ItemID)
{
	InventoryWidget->AddItemToInventory(ItemID);
	return true;
}

bool UPE_InventoryComponent::RemoveItem(UPE_InventoryItemBase* Item)
{
	return true;
}

void UPE_InventoryComponent::UseItem()
{
	UseItem_Server();
}

void UPE_InventoryComponent::UseItem_Server_Implementation()
{
	if (InventoryWidget->IsEmptySlot(SelectedSlotNumber))
	{
		PRINT_LOG(TEXT("Slot is Empty"));
		return;
	}
	// 여기서 아이템 정보를 인벤토리에서 가져오고
	InventoryWidget->UseItem(SelectedSlotNumber);

	// itemthrowcomponent에 접근해서 throw()를 실행
	if (ItemThrowableComponent)
	{
		ItemThrowableComponent->Throw();
	}
	else
	{
		PRINT_LOG(TEXT("ItemThrowableComponent is null"));
	}
}