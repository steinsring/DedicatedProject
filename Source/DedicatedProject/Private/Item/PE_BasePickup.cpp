// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PE_BasePickup.h"
#include "ProjectPlayer.h"
#include "Components/SphereComponent.h"


// Sets default values
APE_BasePickup::APE_BasePickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>("Collision");
	RootComponent = SphereCollision;
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetSphereRadius(200.0f);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(SphereCollision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bReplicates = true; // 액터 복제
}

// Called when the game starts or when spawned
void APE_BasePickup::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &APE_BasePickup::OnBeginOverlap); //동적 멀티캐스트 델리게이트
}

void APE_BasePickup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (const auto Character = Cast<AProjectPlayer>(OtherActor))
	{// 겹치는 액터가 플레이어 캐릭터라면
		Pickup(Character); // 호출
	}
}

// Called every frame
void APE_BasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APE_BasePickup::Pickup_Implementation(AProjectPlayer* OwningCharacter)
{
	SetOwner(OwningCharacter); // 
}

