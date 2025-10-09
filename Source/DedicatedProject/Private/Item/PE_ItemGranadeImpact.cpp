// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PE_ItemGranadeImpact.h"
#include "CharacterCommon.h"
#include "Components/SphereComponent.h"

APE_ItemGranadeImpact::APE_ItemGranadeImpact()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("/Game/Asset/Item/Granade/Granade.Granade"));
	if (StaticMesh.Succeeded())
	{
		GetItemThrowableMesh()->SetStaticMesh(StaticMesh.Object);
	}
}

void APE_ItemGranadeImpact::BeginPlay()
{
	Super::BeginPlay();

	// 폭발시 효과 범위 적용
	if (HasAuthority()) // 서버에서만 판정
	{
		GetWorldTimerManager().SetTimer(DetectTimerHandle, this, &APE_ItemGranadeImpact::Explosion, ExplosionDelay, false);
	}
}

void APE_ItemGranadeImpact::Explosion()
{
	TArray<AActor*> OverlappingActors;
	ItemCollision->GetOverlappingActors(OverlappingActors, ACharacterCommon::StaticClass());	// CharacterCommon만 필터링
	const FVector Origin = GetActorLocation();// 수류탄 위치

	for (AActor* Actor : OverlappingActors)
	{
        if (!Actor || Actor == this) continue;

        // 1) 물리 오브젝트 밀어내기
        TArray<UPrimitiveComponent*> PrimComps;
        Actor->GetComponents<UPrimitiveComponent>(PrimComps);
        for (UPrimitiveComponent* Prim : PrimComps)
        {
            if (Prim && Prim->IsSimulatingPhysics())
            {
                Prim->AddRadialImpulse(
                    Origin,
                    ExplosionRadius,
                    ImpulseStrength,
                    ERadialImpulseFalloff::RIF_Linear,
                    /*bVelChange=*/true
                );
            }
        }

        // 2) 캐릭터는 LaunchCharacter로 튕기기
        if (ACharacterCommon* HitCharacter = Cast<ACharacterCommon>(Actor)) // ← 이름 변경
        {
            const FVector Dir = (HitCharacter->GetActorLocation() - Origin).GetSafeNormal2D();
            FVector Launch = Dir * CharacterLaunchStrength;
            Launch.Z += CharacterUpBoost;

            HitCharacter->LaunchCharacter(Launch, /*XYOverride=*/true, /*ZOverride=*/true);
        }
	}
	Destroy();
}


