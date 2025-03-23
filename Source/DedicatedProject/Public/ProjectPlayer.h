// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectPlayer.generated.h"

UCLASS()
class DEDICATEDPROJECT_API AProjectPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AProjectPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
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

	//캐릭터 스탯 관련
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Data", meta = (AllowPrivateAccess = "true"))
	class UDataTable* CharacterDataTable; //블루프린트에서 데이터 테이블을 직접 참조
	struct FPE_CharacterStats* CharacterStats; // 데이터 테이블에서 단일 행을 참조해 캐릭터 스탯으로 사용
	void UpdateCharacterStats(int32 CharacterLevel);
	FORCEINLINE FPE_CharacterStats* GetCharacterStats() const { return CharacterStats; } //스탯 구조체를 위한 Getter함수

	//달리기 관련
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* ia_Sprint;
	UFUNCTION(Server, Reliable)
	void SprintStart_Server();
	UFUNCTION(Server, Reliable)
	void SprintEnd_Server();
	void SprintStart(const struct FInputActionValue& inputValue);
	void SprintEnd(const struct FInputActionValue& inputValue);

};
