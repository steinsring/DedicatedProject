// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCommon.h"
#include "Common_AnimInstance.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/PE_AIController.h"
#include "BrainComponent.h"
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
	BaseAnimInstance->OnMontageEnded.AddDynamic(this, &ACharacterCommon::OnMontageEnded);
}

// Montage 종료시 Type을 구분해 이후 함수 호출
void ACharacterCommon::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	EMontageType Type =	BaseAnimInstance->GetMontageType(Montage);
	
	switch (Type)
	{
	case EMontageType::Attack:
		OnAttackMontageEnded(Montage, bInterrupted);
		break;
	case EMontageType::Stun:
		EndStun();
		break;
	default: 
		return;
	}
}

void ACharacterCommon::Attack(UAnimMontage* AnimMontage)
{
	if (IsAttacking) return;

	HitActors.Empty();
	if (HasAuthority())
	{
		PlayAttackMontage_Multicast(AnimMontage);
	}
	else
	{
		PlayAttackMontage_Server(AnimMontage);
	}
	IsAttacking = true;
}

void ACharacterCommon::PlayAttackMontage_Server_Implementation(UAnimMontage* AnimMontage)
{
	PlayAttackMontage_Multicast(AnimMontage);
}

void ACharacterCommon::PlayAttackMontage_Multicast_Implementation(UAnimMontage* AnimMontage)
{
	BaseAnimInstance->PlayAttackMontage(AnimMontage);
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
	HealthComp->ApplyDamage_Server(DamageAmount);

	return DamageAmount;
}

void ACharacterCommon::ApplyStun(float Duration)
{
	if (!HasAuthority())
		return;
	
	ApplyStun_NetMulticast(Duration);
}

void ACharacterCommon::ApplyStun_NetMulticast_Implementation(float Duration)
{
	if (bIsStunned) return;
	bIsStunned = true;

	// 1) 현재 진행 중인 공격 끊기
	if (BaseAnimInstance)
	{
		// 공격 Anim 정지 → OnMontageEnded
		BaseAnimInstance->Montage_Stop(0.1f);
	}

	// 2) 공격 중이라면 공격판정/상태 강제 종료
	if (IsAttacking)
	{
		IsAttacking = false;
		OnAttackEnd.Broadcast();
	}

	// 3) 스턴 몽타주 재생
	//  - FullBody 슬롯의 스턴 몽타주여야 공격/로코모션을 확실히 덮어씀
	if (UCommon_AnimInstance* CommonAnim = Cast<UCommon_AnimInstance>(BaseAnimInstance))
	{
		CommonAnim->PlayStunMontage(Duration); // 스턴 Anim 재생
	}

	if (HasAuthority())
	{
		// 4) BT 일시정지
		if (APE_AIController* AI = Cast<APE_AIController>(GetController()))
		{
			if (UBrainComponent* Brain = AI->GetBrainComponent())
			{
				Brain->PauseLogic(TEXT("Stun"));
			}
			AI->StopMovement();
		}
	}
}

void ACharacterCommon::EndStun()
{
	bIsStunned = false;
	if (UCommon_AnimInstance* CommonAnim = Cast<UCommon_AnimInstance>(BaseAnimInstance))
	{
		CommonAnim->EndStunMontage();
	}

	// BT 재개 
	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* Brain = AI->GetBrainComponent())
		{
			Brain->ResumeLogic(TEXT("Stun ended"));
		}
	}
}

