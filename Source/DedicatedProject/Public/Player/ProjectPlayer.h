// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "CharacterCommon.h"
#include "SkillManagerComponent.h"
#include <Components/BoxComponent.h>
#include <Item/PE_BasePickup.h>
#include "ProjectPlayer.generated.h"


class UPE_HPBarWidget;
class UPE_Inventory;
class UPE_InventoryComponent;

UCLASS()
class DEDICATEDPROJECT_API AProjectPlayer : public ACharacterCommon
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	class UPE_AnimInstance* PlayerAnim;

private:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;

	//카메라 관련
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* springArmComp; //카메라 암 위치
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* tpsCamComp; //카메라

	//입력관련 속성
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

	//이동 관련
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Move;
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float walkSpeed = 600;
	//이동방향
	FVector direction;
	void Move(const struct FInputActionValue& inputValue);

	//점프 입력 이벤트 처리
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Jump;
	void InputJump(const struct FInputActionValue& inputValue);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Attack; // 공격 입력 액션
	void InputAttack(const struct FInputActionValue& inputValue);
	//void Attack();

protected:
	//캐릭터 스탯 관련
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Data", meta = (AllowPrivateAccess = "true"))
	class UDataTable* CharacterDataTable; //블루프린트에서 데이터 테이블을 직접 참조
	struct FPE_CharacterStats* CharacterStats; // 데이터 테이블에서 단일 행을 참조해 캐릭터 스탯으로 사용
	void UpdateCharacterStats(int32 CharacterLevel);
	FORCEINLINE FPE_CharacterStats* GetCharacterStats() const { return CharacterStats; } //스탯 구조체를 위한 Getter함수

	//달리기 관련
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

	// 아이템 감지 관련
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* InteractionZone;																				// 아이템 감지 범위 오브젝트
	UPROPERTY()
	APE_BasePickup* FocusedItem;																				// 감지한 아이템

	UFUNCTION()
	void OnItemOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,							// 아이템이 감지 범위 오브젝트와 충돌했을 때
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnItemOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,								// 아이템이 감지 범위 오브젝트에서 벗어났을 때
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Interact;
	void Interact();																							// 플레이어가 E키를 클릭했을 때
	
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPE_InventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UPE_Inventory> InventoryWidgetClass;

	UPROPERTY()
	TObjectPtr<UPE_Inventory> InventoryWidget;

	class UInputAction* ia_SkillTree;

	class UInputAction* ia_SkillChoice;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	TSubclassOf<UPE_HPBarWidget> HPBarWidgetClass; // 플레이어 UI 위젯 컴포넌트

	UPROPERTY()
	TObjectPtr<UPE_HPBarWidget> HPBarWidget; // 플레이어 UI 위젯 인스턴스

	UPROPERTY(VisibleAnywhere, Category = "ItemThrowable")
	TObjectPtr<class UPE_ItemThrowableComponent> ItemThrowable;	// 던지는 아이템

public:
	// Sets default values for this character's properties
	AProjectPlayer();

	UCameraComponent* GetPlayerCamComp() const { return tpsCamComp; }

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hitbox")
	UCapsuleComponent* RightFootHitBox; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skills")
	TObjectPtr<class USkillManagerComponent> SkillManager; // 스킬 매니저 컴포넌트

	UFUNCTION(BlueprintCallable)
	USkillManagerComponent* GetSkillManager() const { return SkillManager; }

	void AddItemToInventory(FName PickupID);

	FORCEINLINE UPE_ItemThrowableComponent* GetItemThrowable() const { return ItemThrowable; }

	float GetAttackPower() const { return AttackPower; }

	float GetDamageMultiplier() const { return DamageMultiplier; }
	float GetAttackPowerMultiplier() const { return AttackPower * AttackPowerMultiplier; }
	float GetSpeedMultiplier() const { return SpeedMultiplier; }

	void SetAttackPower(float Multiplier) { AttackPower *= Multiplier; }
	//void ReturnAttackPower(float AttackPowerMultiplier) { AttackPower /= AttackPowerMultiplier; }

	void SetAttackPowerMultiplier(float NewAttackPowerMultiplier) { AttackPowerMultiplier = NewAttackPowerMultiplier; }
	void SetDamageMultiplier(float NewDamageMultiplier) { DamageMultiplier = NewDamageMultiplier; }
	void SetSpeedMultiplier(float NewSpeedMultiplier) { SpeedMultiplier = NewSpeedMultiplier; }
};
