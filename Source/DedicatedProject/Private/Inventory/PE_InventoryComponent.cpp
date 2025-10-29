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
#include "Player/PE_PlayerState.h"
#include "Player/PE_PlayerController.h"
#include "Inventory/FItemData.h"
#include "Player/ProjectPlayer.h"

// Sets default values for this component's properties
UPE_InventoryComponent::UPE_InventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// IMC 로드 ----------------------------------------------------------------------
	static const TCHAR* IMCPath = TEXT("/Game/Inputs/IMC_Inventory.IMC_Inventory");
	if (UInputMappingContext* IMC = LoadObject<UInputMappingContext>(nullptr, IMCPath))
	{
		InventoryMappingContext = IMC;
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("InventoryMappingContext is NULL"));
	}

	// InputAction 로드 ----------------------------------------------------------------------
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

void UPE_InventoryComponent::InitInputAction(const TObjectPtr<APE_PlayerController> PlayerController)
{
	if (!PlayerController)
	{
		PRINT_ERROR_LOG(TEXT("PlayerController is NULL"));
		return;
	}
	CachedPC = PlayerController;

	// 컴포넌트의 owner가 ProjectPlayer클래스인지 확인
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{// 해당 캐릭터의 컨트롤러를 가져와서
		Subsystem->AddMappingContext(InventoryMappingContext, 1);	// 매핑 컨텍스트 초기화
	}
	else PRINT_ERROR_LOG(TEXT("Subsystem is NULL"));

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{// 마찬가지로 액션 초기화
		for (TObjectPtr<UInputAction> ItemSlotAction : ItemSlotActions)
		{
			EnhancedInputComponent->BindAction(ItemSlotAction, ETriggerEvent::Triggered, this, &UPE_InventoryComponent::ItemSlotSelect);
		}
	}
	else PRINT_ERROR_LOG(TEXT("EnhancedInputComponent is NULL"));
}

// Called when the game starts
void UPE_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UPE_InventoryComponent::ItemSlotSelect(const FInputActionInstance& Instance)
{
	const UInputAction* TriggeredAction = Instance.GetSourceAction();
	APE_PlayerController* PC = CachedPC.Get();

	if (!PC || !PC->IsLocalController())
	{
		PRINT_ERROR_LOG(TEXT("ItemSlotSelect: PC invalid or not local"));
		return;
	}

	for (int32 i = 0; i < NumItemSlots; ++i)
	{
		if (ItemSlotActions[i] == TriggeredAction)
		{
			// 슬롯 번호 i 전달
			PC->ItemSlotSelect(i);
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

bool UPE_InventoryComponent::AddItem(FName ItemID, int32 ItemQuantity, AProjectPlayer* Interactor)
{
	APE_PlayerState* ClientPlayerState = Interactor->GetPlayerState<APE_PlayerState>();
	ClientPlayerState->AddItem(ItemID, ItemQuantity);
	return true;
}

bool UPE_InventoryComponent::RemoveItem(UPE_InventoryItemBase* Item)
{
	return true;
}

void UPE_InventoryComponent::SetInventoryUI(const TArray<FItemData> ServerInventoryData)
{
	//InventoryWidget->SetInventoryData(ServerInventoryData);
}

void UPE_InventoryComponent::UseItem(AProjectPlayer* Interactor)
{
	APE_PlayerState* ClientPlayerState = Interactor->GetPlayerState<APE_PlayerState>();
	ClientPlayerState->UseItem(SelectedSlotNumber);
}

void UPE_InventoryComponent::SpawnItem()
{
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