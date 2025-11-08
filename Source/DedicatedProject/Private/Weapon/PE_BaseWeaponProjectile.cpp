// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/PE_BaseWeaponProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Player/ProjectPlayer.h"
#include "CharacterCommon.h"
#include "Engine/DamageEvents.h"

#include "DedicatedProject.h"


// Sets default values
APE_BaseWeaponProjectile::APE_BaseWeaponProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SphereCollision 생성 및 초기화
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);   // 트리거 전용
	//SphereCollision->SetGenerateOverlapEvents(true);						// 오버랩시 이벤트
	SphereCollision->InitSphereRadius(25.f);
	SphereCollision->SetNotifyRigidBodyCollision(false);					// Hit 이벤트
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Block);
	//SphereCollision->BodyInstance.SetCollisionProfileName("BlockAll");
	SphereCollision->OnComponentHit.AddDynamic(this, &APE_BaseWeaponProjectile::OnHit);
	SphereCollision->SetMobility(EComponentMobility::Movable);
	RootComponent = SphereCollision;
	
	
	// 오브젝트 메시 생성 및 초기화
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	//RootComponent = Mesh;
	Mesh->SetupAttachment(RootComponent);
	//Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);		//물리 시뮬레이션에서만 충돌을 사용하고, 쿼리(Overlap / Hit 테스트)는 비활성화
	Mesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	Mesh->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("/Game/ParagonWraith/FX/Meshes/Heroes/Wraith/SM_Wraith_Drone.SM_Wraith_Drone"));
	if (StaticMesh.Succeeded())
	{
		GetMesh()->SetStaticMesh(StaticMesh.Object);
	}
	//Mesh->SetSimulatePhysics(true);									// 물리 적용
	//SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->IgnoreActorWhenMoving(GetOwner(), true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// ProjectileMovement 생성 및 초기화
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->UpdatedComponent = SphereCollision;
	ProjectileMovement->ProjectileGravityScale = 0;						// 중력 설정
	ProjectileMovement->bRotationFollowsVelocity = true;				// 무기에서 나가도록
	ProjectileMovement->InitialSpeed = 3000;
	ProjectileMovement->MaxSpeed = 3000;
	//ProjectileMovement->Bounciness = 0.6f;								// 탄성(0~1)
	//ProjectileMovement->Friction = 0.2f;								// 마찰
	ProjectileMovement->bShouldBounce = false;							// 바운스 설정
	//ProjectileMovement->BounceVelocityStopSimulatingThreshold = 150.f;	// 너무 느리면 정지
	Mesh->SetRelativeScale3D(FVector(3.0f));
	bReplicates = true;
}

// Called when the game starts or when spawned
void APE_BaseWeaponProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMovement->SetVelocityInLocalSpace(FVector::ForwardVector * ProjectileMovement->InitialSpeed);
	ProjectileMovement->Activate(true);

	// 2) 소유자의 모든 PrimitiveComponent 를 콤포넌트 단위로 무시 (메시/캡슐 포함)
	if (AActor* OwnerActor = GetOwner())
	{
		TArray<UPrimitiveComponent*> Prims;
		OwnerActor->GetComponents<UPrimitiveComponent>(Prims);
		for (UPrimitiveComponent* Pc : Prims)
		{
			if (Pc && SphereCollision) SphereCollision->IgnoreComponentWhenMoving(Pc, true);
		}
	}
}

// Called every frame
void APE_BaseWeaponProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APE_BaseWeaponProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	PRINT_LOG(TEXT("Hit1"));
	if (OtherActor == GetOwner()) return;
	PRINT_LOG(TEXT("Hit2"));
	if (OtherActor && OtherActor != this)
	{
		const FDamageEvent Event(UDamageType::StaticClass());
		ACharacterCommon* HitCharacter = Cast<ACharacterCommon>(OtherActor);
		if (!HitCharacter) return;

		//HitCharacter->ACharacterCommon::TakeDamage(Damage, Event, GetInstigatorController(), this);
		UHealthComponent* HitHealthComp = HitCharacter->FindComponentByClass<UHealthComponent>();
		if (HitHealthComp)
		{
			HitHealthComp->ApplyDamage(Damage);
		}

		HitCharacter->PlayHitSound(OtherActor);
		PRINT_LOG(TEXT("Hit"));
	}

	
	Destroy();
}