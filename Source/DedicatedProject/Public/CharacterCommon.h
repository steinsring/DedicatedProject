// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "GameFramework/Character.h"
#include "CharacterCommon.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class DEDICATEDPROJECT_API ACharacterCommon : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterCommon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	UDataTable* DataTable;

	float AttackPower = 0.0f;
	float MaxSpeed = 0.0f;
	float Speed = 0.0f;
	float MaxHP = 100.0f;

	TSet<AActor*> HitActors;

	UPROPERTY(VisibleAnywhere)
	UHealthComponent* HealthComp;

	UFUNCTION(Server, Reliable)
	void PlayMontage_Server(UAnimMontage* AnimMontage);

	UFUNCTION(NetMulticast, Reliable)
	void PlayMontage_Multicast(UAnimMontage* AnimMontage);

	UFUNCTION(Server, Reliable)
	void StopMontage_Server();

	UFUNCTION(NetMulticast, Reliable)
	void StopMontage_Multicast();


	UFUNCTION(Server, Reliable)
	void PlayHitSound_Server(AActor* HitActor);

	USoundBase* HitSound;

public:
	UFUNCTION(NetMulticast, Reliable)
	void PlayHitSound_Multicast(AActor* HitActor);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	float GetAttackPower() const { return AttackPower; }
	float GetMaxSpeed() const { return MaxSpeed; }
	float GetSpeed() const { return Speed; }

	void Attack(UAnimMontage* AnimMontage);
	FOnAttackEndDelegate OnAttackEnd;

	//UFUNCTION()
	//void Dead();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void ApplyStun(float Duration);	// 스턴이 적용되었을 때 현재 BT, Anim 종료 및 Stun 실행. 내부에서 호출하는 stun 시작점

	UFUNCTION()
	void StopMontage();

	UFUNCTION()
	void HandleDeath();	// 사망 처리

	UFUNCTION()
	void PlayHitSound(AActor* HitActor);

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = true))
	bool IsAttacking;

private:
	UPROPERTY()
	class UCommon_AnimInstance* BaseAnimInstance;

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);	// 현재 실행중인 Montage가 종료됐을 때 호출

	// 스턴 상태
	UPROPERTY(VisibleAnywhere, Category = "Stun")
	bool bIsStunned = false;

	FTimerHandle StunTimer;		// 스턴 시간

	UFUNCTION(NetMulticast, Reliable)
	void ApplyStun_NetMulticast(float Duration);	// 스턴이 적용되었을 때 현재 BT, Anim 종료 및 Stun 실행. 외부에서 호출하는 stun 시작점

	UFUNCTION()
	void EndStun();	// 스턴 종료시

	UFUNCTION()
	void StartFadeOut();

	UFUNCTION()
	void UpdateFadeOut();

	float FadeOpacity;
	FTimerHandle FadeTimerHandle;
	FTimerHandle FadeUpdateTimerHandle;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_IsDead, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDead = false;

	UFUNCTION()
	void OnRep_IsDead();

public:
	void RefreshBaseAnimInstance();

	bool GetIsDead() const { return bIsDead; }
	void SetIsDead(bool bNewState);
};
