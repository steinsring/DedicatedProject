// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PE_ItemPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"


APE_ItemPickup::APE_ItemPickup()
{
    /*
	SphereCollision = CreateDefaultSubobject<USphereComponent>("Collision");
	RootComponent = SphereCollision;
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetSphereRadius(200.0f);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(SphereCollision);
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->SetSimulatePhysics(true);

	bReplicates = true; // 액터 복제
    */
    // 1) 메시를 루트로
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);

    // 물리/충돌 설정
    Mesh->SetMobility(EComponentMobility::Movable);
    Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));   // 또는 QueryAndPhysics 기반 커스텀
    Mesh->SetSimulatePhysics(false);
    Mesh->SetEnableGravity(false);
    Mesh->SetNotifyRigidBodyCollision(true);               // Hit 이벤트 필요 시

    // 2) 오버랩 감지용 구체는 자식으로 (QueryOnly)
    SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    SphereCollision->SetupAttachment(Mesh);
    SphereCollision->SetSphereRadius(200.f);
    SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    SphereCollision->SetGenerateOverlapEvents(true);

    bReplicates = true;
}

void APE_ItemPickup::BeginPlay()
{
    Super::BeginPlay();

    if (!HasAuthority())
    {
        // 서버가 아니면 (클라이언트면) 위치 조정 안함
        return;
    }

    FVector Start = GetActorLocation() + FVector(0, 0, 50);    // 약간 위에서 시작
    FVector End = GetActorLocation() - FVector(0, 0, 1000);  // 아래로 레이 쏘기

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility,
        Params
    );

    if (bHit)
    {
        // 아이템을 바닥 Hit 위치로 이동
        FVector GroundLocation = Hit.ImpactPoint;
        SetActorLocation(GroundLocation);

        // 필요하면 회전도 바닥 Normal 기준으로 세팅 가능
        // SetActorRotation(Hit.ImpactNormal.Rotation());
    }
}

