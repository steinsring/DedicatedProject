// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PE_ItemThrowableComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Item/PE_ItemThrowable.h"
#include "Item/PE_ItemGranadeEMP.h"
#include "DedicatedProject.h"
#include "Player/ProjectPlayer.h"


// Sets default values for this component's properties
UPE_ItemThrowableComponent::UPE_ItemThrowableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ItemThrowable = APE_ItemGranadeEMP::StaticClass();
}


// Called when the game starts
void UPE_ItemThrowableComponent::BeginPlay()
{
	Super::BeginPlay();
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
	else
	{
		PRINT_LOG(TEXT("ItemThrowable is null"));
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

