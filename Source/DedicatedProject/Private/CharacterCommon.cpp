// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCommon.h"
#include "Common_AnimInstance.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

#include "DedicatedProject.h"

// Sets default values
ACharacterCommon::ACharacterCommon()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthStat"));

	IsAttacking = false;
}

// Called when the game starts or when spawned
void ACharacterCommon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterCommon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACharacterCommon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	BaseAnimInstance = Cast<UCommon_AnimInstance>(GetMesh()->GetAnimInstance());

	if (!BaseAnimInstance)
	{
		PRINT_LOG(TEXT("BaseAnim is NULL"));
		return;
	}
	BaseAnimInstance->OnMontageEnded.AddDynamic(this, &ACharacterCommon::OnAttackMontageEnded);
}

void ACharacterCommon::Attack(UAnimMontage* AnimMontage)
{
	if (IsAttacking) return;

	HitActors.Empty();
	BaseAnimInstance->PlayAttackMontage(AnimMontage);
	IsAttacking = true;
}

void ACharacterCommon::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsAttacking = false;
	OnAttackEnd.Broadcast();
}

void ACharacterCommon::OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void ACharacterCommon::SetHitbox(ECollisionEnabled::Type CollisionEnabled, UCapsuleComponent* HitBox)
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

float ACharacterCommon::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return HealthComp->ApplyDamage(DamageAmount);
}

