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
    Mesh->SetSimulatePhysics(true);
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

