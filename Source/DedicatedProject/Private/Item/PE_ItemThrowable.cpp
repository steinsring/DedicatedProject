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

	// 오브젝트 메시 생성 및 초기화
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>("ItemMesh");
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);		//물리 시뮬레이션에서만 충돌을 사용하고, 쿼리(Overlap / Hit 테스트)는 비활성화
	ItemMesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	ItemMesh->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	ItemMesh->BodyInstance.SetCollisionProfileName("BlockAll");
	ItemMesh->SetSimulatePhysics(true);									// 물리 적용
	SetRootComponent(ItemMesh);

	// SphereCollision 생성 및 초기화
	ItemCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	ItemCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);   // 트리거 전용
	ItemCollision->SetGenerateOverlapEvents(true);
	ItemCollision->InitSphereRadius(700.f);
	ItemCollision->SetGenerateOverlapEvents(true);						// 오버랩시 이벤트
	ItemCollision->SetNotifyRigidBodyCollision(false);					// Hit 이벤트
	//ItemCollision->OnComponentHit.AddDynamic(this, &APE_ItemThrowable::OnHit);
	ItemCollision->SetupAttachment(RootComponent);
	

	// ProjectileMovement 생성 및 초기화
	/*
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->UpdatedComponent = ItemMesh;
	ProjectileMovement->ProjectileGravityScale = 1.f;					// 중력 설정	
	ProjectileMovement->bRotationFollowsVelocity = false;				// 진행 방향을 바라보게
	ProjectileMovement->InitialSpeed = 3000;
	ProjectileMovement->MaxSpeed = 3000;
	ProjectileMovement->Bounciness = 0.6f;								// 탄성(0~1)
	ProjectileMovement->Friction = 0.2f;								// 마찰
	ProjectileMovement->bShouldBounce = true;							// 바운스 설정
	ProjectileMovement->BounceVelocityStopSimulatingThreshold = 150.f;	// 너무 느리면 정지
	*/
	bReplicates = true;
}

// Called when the game starts or when spawned
void APE_ItemThrowable::BeginPlay()
{
	Super::BeginPlay();

	// 1) 초기 선형 속도: 액터 Forward 방향 기준
	const FVector Forward = GetActorForwardVector();
	FVector Velocity = Forward * 3000.f;

	// 2) 발사자 속도 상속

	if (const APawn* InstigatorPawn = Cast<APawn>(GetOwner()))
	{
		Velocity += InstigatorPawn->GetVelocity();
	}
	else if (const AActor* OwnerActor = GetOwner())
	{
		Velocity += OwnerActor->GetVelocity();
	}


	// 3) 적용
	ItemMesh->SetPhysicsLinearVelocity(Velocity, /*bAddToCurrent=*/false);
}

// Called every frame
void APE_ItemThrowable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

