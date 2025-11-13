// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "CharacterCommon.h"
#include "PE_Interactable.h"
#include "SkillManagerComponent.h"
#include <Components/BoxComponent.h>
#include "ProjectPlayer.generated.h"

class UPE_Inventory;
class UPE_InventoryComponent;

UCLASS()
class DEDICATEDPROJECT_API AProjectPlayer : public ACharacterCommon, public IPE_Interactable
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;   // 서버에서만 호출되는 소유자 지정 완료 콜백.
	virtual void OnRep_Controller() override;                        // 클라이언트에서 호출되는 Controller 복제본 후 클라의 Pawn에서 콜백.

	UPROPERTY()
	class UPE_AnimInstance* PlayerAnim;

private:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;

private:
	//카메라 컴포넌트 --------------------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* springArmComp; //카메라 암 위치
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* tpsCamComp; //카메라

public:
	FORCEINLINE UCameraComponent* GetPlayerCamComp() const { return tpsCamComp; }

	//리스폰 관련	------------------------------------------------------------------------------

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Power")
	bool bIsPowerOn = true;

	UFUNCTION(NetMulticast, Reliable)
	void SetPowerState_Multicast(bool bNewPowerState);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void RequestRespawn_Server(APlayerState* DeadPlayerState, AProjectPlayer* TargetDummy);

	UFUNCTION()
	void OpenRespawnUI(AProjectPlayer* TargetDummy);

private:
	// 플레이어 화면 조작--------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* imc_ProjectPlayer; //만들어둔 IMC_ProjectPlayer
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_LookUp; // 만들어둔 IA_LookUp
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Turn; // 만들어둔 IA_Turn

	//좌우 회전 입력 처리
	void Turn(const struct FInputActionValue& inputValue);
	//상하 회전 입력 처리
	void LookUp(const struct FInputActionValue& inputValue);

private:
	// 플레이어 이동 --------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Move;
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float walkSpeed = 600;
	FVector direction;
	void Move(const struct FInputActionValue& inputValue);

	// 점프 --------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Jump;
	void InputJump(const struct FInputActionValue& inputValue);

	// 공격 --------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Attack; // 공격 입력 액션

	void InputAttack(const struct FInputActionValue& inputValue);

	UPROPERTY(VisibleAnywhere, Category = "WeaponProjectile")
	TObjectPtr<class UPE_WeaponProjectileComponent> WeaponProjectileComponent;

public:
	FORCEINLINE UPE_WeaponProjectileComponent* GetWeaponProjectileComponent() const { return WeaponProjectileComponent; }

protected:
	//캐릭터 스탯 관련 --------------------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Data", meta = (AllowPrivateAccess = "true"))
	class UDataTable* CharacterDataTable; //블루프린트에서 데이터 테이블을 직접 참조
	struct FPE_CharacterStats* CharacterStats; // 데이터 테이블에서 단일 행을 참조해 캐릭터 스탯으로 사용
	void UpdateCharacterStats(int32 CharacterLevel);
	FORCEINLINE FPE_CharacterStats* GetCharacterStats() const { return CharacterStats; } //스탯 구조체를 위한 Getter함수

	virtual void Interact(class AActor* Interactor) override; // IPE_Interactable 인터페이스 구현

protected:
	//달리기 관련 --------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* ia_Sprint;
private:
	UFUNCTION(Server, Reliable)
	void SprintStart_Server();
	UFUNCTION(Server, Reliable)
	void SprintEnd_Server();
	void SprintStart(const struct FInputActionValue& inputValue);
	void SprintEnd(const struct FInputActionValue& inputValue);

	UFUNCTION(NetMulticast, Reliable)
	void SprintStart_Client();
	UFUNCTION(NetMulticast, Reliable)
	void SprintEnd_Client();

private:
	// 상호작용 --------------------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, Category = "Interact")
	UBoxComponent* InteractionZone;																				// 아이템 감지 범위 오브젝트
	UPROPERTY(VisibleAnywhere, Category = "Interact")
	TScriptInterface<class IPE_Interactable> FocusedItem;																				// 감지한 아이템

	UFUNCTION()
	void OnItemOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,							// 아이템이 감지 범위 오브젝트와 충돌했을 때
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnItemOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,								// 아이템이 감지 범위 오브젝트에서 벗어났을 때
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Interact;
	void Interact();													// 플레이어가 E키를 클릭했을 때
	
private:
	// 아이템 사용 ------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_ItemUse;

	void ItemUse(const FInputActionValue& inputValue);
	
	// --------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Skill_Augment;
	void InputAugmentSkill(const struct FInputActionValue& inputValue);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Skill_Override;
	void WhileHoldingOverrideSkill(const struct FInputActionValue& inputValue);
	void OnReleaseOverrideSkill(const struct FInputActionValue& inputValue);
	
	float AttackPowerMultiplier = 1.0f;
	float DamageMultiplier = 1.0f;
	float SpeedMultiplier = 1.0f;

	class UInputAction* ia_SkillTree;

	class UInputAction* ia_SkillChoice;
	 
	// Inventory  -------------------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPE_InventoryComponent> InventoryComponent;

public:
	void AddItemToInventory(FName ID, int32 Quantity);
	FORCEINLINE UPE_InventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	
	// Throwable -------------------------------------------------------------------------
private:
	UPROPERTY(VisibleAnywhere, Category = "ItemThrowable")
	TObjectPtr<class UPE_ItemThrowableComponent> ItemThrowable;	

public:
	FORCEINLINE UPE_ItemThrowableComponent* GetItemThrowable() const { return ItemThrowable; }

	// Fuel -------------------------------------------------------------------------
private:
	UPROPERTY(VisibleAnywhere, Category = "Fuel")
	TObjectPtr<class UPE_FuelComponent> FuelComponent;	// FuelComponent

	// Light -------------------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, Category = "Light")
	TObjectPtr<class USpotLightComponent> SpotLightComp;

public:
	// Sets default values for this character's properties
	AProjectPlayer();

	// 스킬 관련 -------------------------------------------------------------------------
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hitbox")
	UCapsuleComponent* RightFootHitBox; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skills")
	TObjectPtr<class USkillManagerComponent> SkillManager; // 스킬 매니저 컴포넌트

	UFUNCTION(BlueprintCallable)
	USkillManagerComponent* GetSkillManager() const { return SkillManager; }

	// Augment Skill 관련 ---------------------------------------------------------------
	float GetAttackPower() const { return AttackPower; }

	float GetDamageMultiplier() const { return DamageMultiplier; }
	float GetAttackPowerMultiplier() const { return AttackPower * AttackPowerMultiplier; }
	float GetSpeedMultiplier() const { return SpeedMultiplier; }

	void SetAttackPower(float Multiplier) { AttackPower *= Multiplier; }
	//void ReturnAttackPower(float AttackPowerMultiplier) { AttackPower /= AttackPowerMultiplier; }

	void SetAttackPowerMultiplier(float NewAttackPowerMultiplier) { AttackPowerMultiplier = NewAttackPowerMultiplier; }
	void SetDamageMultiplier(float NewDamageMultiplier) { DamageMultiplier = NewDamageMultiplier; }
	void SetSpeedMultiplier(float NewSpeedMultiplier) { SpeedMultiplier = NewSpeedMultiplier; }

private:
	//UFUNCTION(Server, Reliable)
	//void Die_Server();

	//UFUNCTION(NetMulticast, Reliable)
	//void Die_Multicast();

public:
	void Die();
};
