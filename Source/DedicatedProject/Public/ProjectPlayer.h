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

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Move;
	//이동 속도
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float walkSpeed = 600;
	//이동방향
	FVector direction;

	void Move(const struct FInputActionValue& inputValue);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Jump;
	//점프 입력 이벤트 처리
	void InputJump(const struct FInputActionValue& inputValue);
};
