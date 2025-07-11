// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Enemy/PE_AIController.h"
#include "Enemy/PE_AnimInstance.h"

#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"

#include "Enemy/PE_ToiletMechStats.h"
#include "Kismet/GameplayStatics.h"

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
	PEAnim = Cast<UPE_AnimInstance>(GetMesh()->GetAnimInstance());

	PEAnim->OnMontageEnded.AddDynamic(this, &AEnemy::OnAttackMontageEnded);
}

void AEnemy::Attack()
{
	if (IsAttacking) return;

	PEAnim->PlayAttackMontage();
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

