// Fill out your copyright notice in the Description page of Project Settings.


#include "ToiletMech.h"
#include "PE_AIController.h"


// Sets default values
AToiletMech::AToiletMech()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//스켈레탈 메쉬를 구조체로 불러와서
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Game/Fab/SciFi_ToiletMech/Mesh/SK_SciFi_ToiletMech_Skin4.SK_SciFi_ToiletMech_Skin4"));
	if (TempMesh.Succeeded()) { //에셋 로드가 성공했다면
		/*
		GetMesh로 Charactor 클래스의 Mesh를 불러오고 Mesh에 있는 SetSkeletalMesh를 호출
		원하는 Mesh의 Object를 인수로 보내 SkeletalMesh를 설정해준다.
		*/
		GetMesh()->SetSkeletalMesh(TempMesh.Object);

		//마찬가지로 Mesh의 위치와 회전을 설정해 준다.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));

		//ProjectPlayer 불루프린트 클래스에 애니메이션 블루프린트를 세팅해준다.
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		static ConstructorHelpers::FClassFinder<UAnimInstance> MechAnim(TEXT("/Game/BluePrints/AB_ToiletAnimBlueprint.AB_ToiletAnimBlueprint_C"));
		if (MechAnim.Succeeded())
		{
			GetMesh()->SetAnimInstanceClass(MechAnim.Class);
		}
		//이렇게 하면 블루프린트에 오류가 생겨 삭제하고 재생성했을때 자동으로 설정해준다.

		//ai controller 세팅(만약 플레이어가 조종하지 않는 캐릭터라면 ai_controller의 지배를 받게 된다.)
		AIControllerClass = APE_AIController::StaticClass();
		AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	}
}

// Called when the game starts or when spawned
void AToiletMech::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AToiletMech::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AToiletMech::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

