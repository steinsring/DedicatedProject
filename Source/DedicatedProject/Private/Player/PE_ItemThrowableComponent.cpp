// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PE_ItemThrowableComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Item/PE_ItemThrowable.h"
#include "DedicatedProject.h"
#include "Player/ProjectPlayer.h"


// Sets default values for this component's properties
UPE_ItemThrowableComponent::UPE_ItemThrowableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ItemThrowable = APE_ItemThrowable::StaticClass();

	// IMC 로드
	static const TCHAR* IMCPath = TEXT("/Game/Inputs/IMC_ItemThrowable.IMC_ItemThrowable");
	if (UInputMappingContext* IMC = LoadObject<UInputMappingContext>(nullptr, IMCPath))
	{
		ThrowMappingContext = IMC;
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("ThrowMappingContext is NULL"));
	}

	// IA 로드
	static const TCHAR* IAPath = TEXT("/Game/Inputs/IA_Throw.IA_Throw");
	if (UInputAction* IA = LoadObject<UInputAction>(nullptr, IAPath))
	{
		ThrowAction = IA;
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("ThrowInputAction is NULL"));
	}
}


// Called when the game starts
void UPE_ItemThrowableComponent::BeginPlay()
{
	Super::BeginPlay();

	const AProjectPlayer* Character = Cast<AProjectPlayer>(GetOwner());
	if (!Character)
	{
		return;
	}

	if (const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{// 컴포넌트의 owner가 ProjectPlayer클래스인지 확인
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{// 해당 캐릭터의 컨트롤러를 가져와서
			Subsystem->AddMappingContext(ThrowMappingContext, 1);	// 매핑 컨텍스트 초기화
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{// 마찬가지로 액션 초기화
			EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &UPE_ItemThrowableComponent::Throw);
		}
	}
	
}

void UPE_ItemThrowableComponent::Throw()
{
	Throw_Server();
}

//서버 RPC
void UPE_ItemThrowableComponent::Throw_Server_Implementation()
{
	if (ItemThrowable)
	{
		AProjectPlayer* const Character = Cast<AProjectPlayer>(GetOwner());	//컴포넌트의 소유자(Owner)를 AProjectPlayer로 캐스팅
		const FVector ItemSpawnLocation = GetComponentLocation();
		const FRotator ItemSpawnRotation = GetComponentRotation();
		FActorSpawnParameters ItemSpawnParams = FActorSpawnParameters();	
		ItemSpawnParams.Owner = GetOwner();									// 투사체의 소유자 설정
		ItemSpawnParams.Instigator = Character;								// 투사체를 던진 플레이어 설정

		GetWorld()->SpawnActor<APE_ItemThrowable>(ItemThrowable, ItemSpawnLocation, ItemSpawnRotation, ItemSpawnParams);
	}
}

// 스폰하는 아이템 변경
void UPE_ItemThrowableComponent::SetProjectileClass(TSubclassOf<class APE_ItemThrowable> NewItemThrowable)
{
	ItemThrowable = NewItemThrowable;
}

// Called every frame
void UPE_ItemThrowableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

