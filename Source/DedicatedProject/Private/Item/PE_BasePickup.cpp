// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PE_BasePickup.h"
#include "Player/ProjectPlayer.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "DedicatedProject.h"


// Sets default values
APE_BasePickup::APE_BasePickup()
{
	SphereCollision = CreateDefaultSubobject<USphereComponent>("Collision");
	RootComponent = SphereCollision;
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetSphereRadius(200.0f);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(SphereCollision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetGenerateOverlapEvents(true);

	bReplicates = true; // 액터 복제
}

// Called when the game starts or when spawned
void APE_BasePickup::BeginPlay()
{
	Super::BeginPlay();

	//SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &APE_BasePickup::Interact); //동적 멀티캐스트 델리게이트
}

void APE_BasePickup::Interact(AActor* Interactor)
{
	PRINT_LOG(TEXT("Item : Test"));
	if (HasAuthority()) // 서버가 아니면 서버에게 처리 위임
	{
		PRINT_LOG(TEXT("Item : Server"));
		ServerInteract(Interactor);
		return;
	}
}

void APE_BasePickup::ServerInteract_Implementation(AActor* Interactor)
{
	// 플레이어가 E 키를 눌렀을 때 픽업
	if (AProjectPlayer* Player = Cast<AProjectPlayer>(Interactor))
	{
		PRINT_LOG(TEXT("Item : Destroy"));
		Destroy();
	}
}

