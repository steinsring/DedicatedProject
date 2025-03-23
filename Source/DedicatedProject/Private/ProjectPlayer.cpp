// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectPlayer.h"
#include <GameFramework/SpringArmComponent.h> //3인칭 카메라암
#include <Camera/CameraComponent.h> // 카메라
#include "EnhancedInputSubsystems.h" //EnhancedInput 사용을 위함
#include "EnhancedInputComponent.h"
#include "PE_CharacterStats.h"
#include "Engine/DataTable.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AProjectPlayer::AProjectPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//스켈레탈 메쉬를 구조체로 불러와서
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin'"));
	if (TempMesh.Succeeded()) { //에셋 로드가 성공했다면
		/*
		GetMesh로 Charactor 클래스의 Mesh를 불러오고 Mesh에 있는 SetSkeletalMesh를 호출
		원하는 Mesh의 Object를 인수로 보내 SkeletalMesh를 설정해준다.
		*/
		GetMesh()->SetSkeletalMesh(TempMesh.Object); 

		//마찬가지로 Mesh의 위치와 회전을 설정해 준다.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));

		//이렇게 하면 블루프린트에 오류가 생겨 삭제하고 재생성했을때 자동으로 설정해준다.

		// 3. TPS 카메라를 붙인다.
		// 3-1. SpringArm 컴포넌트 붙이기
		springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
		springArmComp->SetupAttachment(RootComponent); // 루트컴포넌트의 자식으로
		springArmComp->SetRelativeLocation(FVector(0, 70, 90)); //컴포넌트 위치, 상대좌표
		springArmComp->TargetArmLength = 400; // 타깃과의 지정 거리
		springArmComp->bUsePawnControlRotation = true; //입력의 회전 설정

		// 3-2. Camera 컴포넌트
		tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
		tpsCamComp->SetupAttachment(springArmComp); //카메라는 카메라 암에
		tpsCamComp->bUsePawnControlRotation = false;//입력의 회전 설정

		bUseControllerRotationYaw = true;//입력의 회전 설정
	}
}

// Called when the game starts or when spawned
void AProjectPlayer::BeginPlay()
{
	Super::BeginPlay();

	//EnhancedInputSystem에 imc_TPS등록
	//오류가 난다면 TPSProjec.Build.cs에 모듈에서 EnhancedInput을 추가해 주자.
	auto pc = Cast<APlayerController>(Controller); //현재 플레이어의 APlayerController를 가져온다.
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer()); //입력 서브시스템을 가져와서
		if (subsystem)
		{
			subsystem->AddMappingContext(imc_ProjectPlayer, 0); //입력 컨텍스트에 등록한다.
		}
	}

	UpdateCharacterStats(1); //캐릭터 스탯 설정
	
}

// Called every frame
void AProjectPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//플레이어 이동 처리
	//등속 운동
	//P(결과 위치) = P_0(현재위치) + vt
	direction = FTransform(GetControlRotation()).TransformVector(direction); //이동방향을 컨트롤 방향 기준으로 변환
	/*FVector P0 = GetActorLocation();
	FVector vt = direction * walkSpeed * DeltaTime;
	FVector P = P0 + vt;
	SetActorLocation(P);*/
	AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

// Called to bind functionality to input
void AProjectPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput)
	{
		// BindAction(연결할 Input Action, 입력이벤트, 연결할 함수가 있는 클래스, 연결할 함수의 주소값)
		PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &AProjectPlayer::Turn);
		PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &AProjectPlayer::LookUp);
		PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &AProjectPlayer::Move);
		PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this, &AProjectPlayer::InputJump);
		PlayerInput->BindAction(ia_Sprint, ETriggerEvent::Started, this, &AProjectPlayer::SprintStart);
		PlayerInput->BindAction(ia_Sprint, ETriggerEvent::Completed, this, &AProjectPlayer::SprintEnd);
	}
}


void AProjectPlayer::Turn(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerYawInput(value); // 입력 값의 float값을 가져와 z축 회전에 사용
}

void AProjectPlayer::LookUp(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerPitchInput(value);
}

void AProjectPlayer::Move(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();
	//상하 입력 이벤트 처리
	direction.X = value.X;
	//좌우 입력 이벤트 처리
	direction.Y = value.Y;
}

void AProjectPlayer::InputJump(const struct FInputActionValue& inputValue)
{
	Jump();
}
 
void AProjectPlayer::UpdateCharacterStats(int32 CharacterLevel) {
	// 캐릭터가 달리는중 스탯이 변경되는경우 걷기속도로 되돌아가는 문제를 해결하기 위함
	auto IsSprinting = false;
	if (GetCharacterStats())
		IsSprinting = GetCharacterMovement()->MaxWalkSpeed == GetCharacterStats()->SprintSpeed;

	if (CharacterDataTable) //데이터 테이블이 참조돼었는지 확인
	{
		TArray<FPE_CharacterStats*> CharacterStatsRows;
		CharacterDataTable->GetAllRows<FPE_CharacterStats>(TEXT("ProjectPlayer"), CharacterStatsRows); //테이블의 모든 행을 지역배열로 가져온다.

		if (CharacterStatsRows.Num() > 0) //데이터 테이블에 행이 하나 이상이면
		{
			const auto NewCharacterLevel = FMath::Clamp(CharacterLevel, 1, CharacterStatsRows.Num()); // 캐릭터 레벨에서 1을 뺀 행을 가져온다. Clamp()로 사용가능한 행보다 높은 값을 가져오지 않게함.
			CharacterStats = CharacterStatsRows[NewCharacterLevel - 1];

			GetCharacterMovement()->MaxWalkSpeed = GetCharacterStats()->WalkSpeed; //WalkSpeed 열의 값을가져와 캐릭터의 MaxWalkSpeed프로퍼티에 할당

			if (IsSprinting)
				SprintStart_Server(); //달리는중 올바르게 업데이트하기 위함
		}
	}
}
void AProjectPlayer::SprintStart(const struct FInputActionValue& inputValue)
{
	SprintStart_Server();
}

void AProjectPlayer::SprintEnd(const struct FInputActionValue& inputValue)
{
	SprintEnd_Server();
}

void AProjectPlayer::SprintStart_Server_Implementation()
{
	if (GetCharacterStats())
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStats()->SprintSpeed;
}

void AProjectPlayer::SprintEnd_Server_Implementation()
{
	if (GetCharacterStats())
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStats()->WalkSpeed;
}

