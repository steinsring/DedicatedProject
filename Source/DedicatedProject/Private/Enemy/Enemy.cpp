// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"
#include "Enemy/PE_AIController.h"
#include "Enemy/Enemy_AnimInstance.h"

#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"

#include "Enemy/PE_ToiletMechStats.h"
#include "Kismet/GameplayStatics.h"

#include "DedicatedProject.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthStat"));

	IsAttacking = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	EnemyAnim = Cast<UEnemy_AnimInstance>(GetMesh()->GetAnimInstance());

	if (!EnemyAnim)
	{
		PRINT_LOG(TEXT("EnemyAnim is NULL"));
		return;
	}
	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemy::OnAttackMontageEnded);
}

void AEnemy::Attack(UAnimMontage* AnimMontage)
{
	if (IsAttacking) return;

	EnemyAnim->PlayAttackMontage(AnimMontage);
	IsAttacking = true;
}

void AEnemy::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsAttacking = false;
	OnAttackEnd.Broadcast();
}

void AEnemy::AttackCheck()
{
}

void AEnemy::OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || OtherActor == this || HitActors.Contains(OtherActor)) return;

	HitActors.Add(OtherActor);

	UE_LOG(LogTemp, Log, TEXT("Overlapped with: %s | Component: %s"), *OtherActor->GetName(), *OtherComp->GetName());

	UGameplayStatics::ApplyDamage(
		OtherActor,
		AttackPower,
		GetController(),
		this,
		nullptr
	);
}

void AEnemy::SetHitbox(ECollisionEnabled::Type CollisionEnabled, UCapsuleComponent* HitBox)
{
	if (CollisionEnabled == ECollisionEnabled::NoCollision)
		HitActors.Empty();

	if (HitBox)
	{
		HitBox->SetCollisionEnabled(CollisionEnabled);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HitBox is NULL"));
	}
}

