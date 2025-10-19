// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PE_ItemGranadeEMP.h"
#include "CharacterCommon.h"
#include "Components/SphereComponent.h"

APE_ItemGranadeEMP::APE_ItemGranadeEMP()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("/Game/Asset/Item/Granade/Granade.Granade"));
	if (StaticMesh.Succeeded())
	{
		GetItemThrowableMesh()->SetStaticMesh(StaticMesh.Object);
	}

	bReplicates = true;
}

void APE_ItemGranadeEMP::BeginPlay()
{
	Super::BeginPlay();

	// 폭발시 효과 범위 적용
	if (HasAuthority()) // 서버에서만 판정
	{
		GetWorldTimerManager().SetTimer(DetectTimerHandle, this, &APE_ItemGranadeEMP::Explosion, ExplosionDelay, false);
	}
}

void APE_ItemGranadeEMP::Explosion()
{
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




