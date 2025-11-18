// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PE_ItemGranadeEMP.h"
#include "CharacterCommon.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

APE_ItemGranadeEMP::APE_ItemGranadeEMP()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("/Game/Asset/Item/Granade/Granade.Granade"));
	if (StaticMesh.Succeeded())
	{
		GetItemThrowableMesh()->SetStaticMesh(StaticMesh.Object);
	}



	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionEffectFinder(
		TEXT("/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_HitWorld.P_Wraith_Primary_HitWorld")
	);

	if (ExplosionEffectFinder.Succeeded())
	{
		ExplosionEffect = ExplosionEffectFinder.Object;
	}

	bReplicates = true;
}

void APE_ItemGranadeEMP::BeginPlay()
{
	Super::BeginPlay();

	ItemMesh->SetRelativeScale3D(FVector(0.5f));
	ItemCollision->InitSphereRadius(1000.f);

	// 폭발시 효과 범위 적용
	if (HasAuthority()) // 서버에서만 판정
	{
		GetWorldTimerManager().SetTimer(DetectTimerHandle, this, &APE_ItemGranadeEMP::Explosion, ExplosionDelay, false);
	}
}

void APE_ItemGranadeEMP::Multicast_PlayExplosionEffects_Implementation()
{
	if (ExplosionEffect)
	{
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = FRotator::ZeroRotator;

		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionEffect,
			SpawnLocation,
			SpawnRotation,
			true // AutoDestroy
		);
	}
}

void APE_ItemGranadeEMP::Explosion()
{
	// 모든 클라이언트에 이펙트 재생
	Multicast_PlayExplosionEffects();

	TArray<AActor*> OverlappingActors;
	ItemCollision->GetOverlappingActors(OverlappingActors, ACharacterCommon::StaticClass());	// CharacterCommon만 필터링

	for (AActor* Actor : OverlappingActors)
	{
		if (ACharacterCommon* Character = Cast<ACharacterCommon>(Actor))
		{
			Character->ApplyStun(StunTime);	// 범위안의 모든 캐릭터 스턴 실행
		}
	}
	Destroy();
}






