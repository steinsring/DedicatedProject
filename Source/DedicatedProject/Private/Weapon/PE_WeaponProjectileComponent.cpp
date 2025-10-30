// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/PE_WeaponProjectileComponent.h"
#include "Weapon/PE_BaseWeaponProjectile.h"
#include "Player/ProjectPlayer.h"
#include "DedicatedProject.h"


// Sets default values for this component's properties
UPE_WeaponProjectileComponent::UPE_WeaponProjectileComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ProjectileClass = APE_BaseWeaponProjectile::StaticClass();
}


// Called when the game starts
void UPE_WeaponProjectileComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPE_WeaponProjectileComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPE_WeaponProjectileComponent::Fire()
{
	Fire_Server();
}

void UPE_WeaponProjectileComponent::Fire_Server_Implementation()
{
	if (ProjectileClass)
	{
		AProjectPlayer* const Character = Cast<AProjectPlayer>(GetOwner());	//컴포넌트의 소유자(Owner)를 AProjectPlayer로 캐스팅
		const FVector ItemSpawnLocation = GetComponentLocation();
		const FRotator ItemSpawnRotation = GetComponentRotation();
		FActorSpawnParameters ItemSpawnParams = FActorSpawnParameters();
		ItemSpawnParams.Owner = GetOwner();									// 투사체의 소유자 설정
		ItemSpawnParams.Instigator = Character;								// 투사체를 던진 플레이어 설정

		GetWorld()->SpawnActor<APE_BaseWeaponProjectile>(ProjectileClass, ItemSpawnLocation, ItemSpawnRotation, ItemSpawnParams);
	}
	else
	{
		PRINT_LOG(TEXT("ProjectileClass is null"));
	}
}

