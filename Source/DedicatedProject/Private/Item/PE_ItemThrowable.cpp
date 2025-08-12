// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PE_ItemThrowable.h"
#include "Player/ProjectPlayer.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
APE_ItemThrowable::APE_ItemThrowable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SphereCollision 생성 및 초기화
	ItemCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	ItemCollision->SetGenerateOverlapEvents(true);
	ItemCollision->InitSphereRadius(10.f);
	ItemCollision->BodyInstance.SetCollisionProfileName("BlockAll");
	ItemCollision->SetNotifyRigidBodyCollision(true);								// Hit 이벤트
	//ItemCollision->OnComponentHit.AddDynamic(this, &APE_ItemThrowable::OnHit);
	SetRootComponent(ItemCollision);

	// 오브젝트 메시 생성 및 초기화
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>("ItemMesh");
	ItemMesh->SetupAttachment(RootComponent);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);		//물리 시뮬레이션에서만 충돌을 사용하고, 쿼리(Overlap / Hit 테스트)는 비활성화
	ItemMesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	ItemMesh->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("/Game/Asset/Item/Star_Citizen_Workflow_to_Sketchfab__-_Decals_3/Fuel_StaticMesh.Fuel_StaticMesh"));
	if (StaticMesh.Succeeded())
	{
		GetItemThrowableMesh()->SetStaticMesh(StaticMesh.Object);
	}

	// ProjectileMovement 생성 및 초기화
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->UpdatedComponent = ItemCollision;
	ProjectileMovement->ProjectileGravityScale = 1.f;					// 중력 설정	
	ProjectileMovement->bRotationFollowsVelocity = true;				// 진행 방향을 바라보게
	ProjectileMovement->InitialSpeed = 3000;
	ProjectileMovement->MaxSpeed = 3000;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->Bounciness = 0.6f;								// 탄성(0~1)
	ProjectileMovement->Friction = 0.2f;								// 마찰
	ProjectileMovement->bShouldBounce = true;							// 바운스 설정
	ProjectileMovement->BounceVelocityStopSimulatingThreshold = 150.f;	// 너무 느리면 정지

	bReplicates = true;
}

// Called when the game starts or when spawned
void APE_ItemThrowable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APE_ItemThrowable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

