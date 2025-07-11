// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/ProjectPlayer.h"
#include "DedicatedProject.h"
#include "Engine/DataTable.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h" //EnhancedInput 사용을 위함
#include "GameFramework/CharacterMovementComponent.h"
#include "Inventory/PE_InventoryComponent.h"
#include "Enemy/PE_AIController.h"
#include "Player/PE_CharacterStats.h"
#include "UI/PE_Inventory.h"
#include <Camera/CameraComponent.h> // 카메라
#include <GameFramework/SpringArmComponent.h> //3인칭 카메라암
#include "Blueprint/UserWidget.h"


// Sets default values
AProjectPlayer::AProjectPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//스켈레탈 메쉬를 구조체로 불러와서
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin'"));
	//ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT(""));
	if (TempMesh.Succeeded())
	{ //에셋 로드가 성공했다면
		/*
		GetMesh로 Charactor 클래스의 Mesh를 불러오고 Mesh에 있는 SetSkeletalMesh를 호출
		원하는 Mesh의 Object를 인수로 보내 SkeletalMesh를 설정해준다.
		*/
		GetMesh()->SetSkeletalMesh(TempMesh.Object);

		//마찬가지로 Mesh의 위치와 회전을 설정해 준다.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));

		//ProjectPlayer 불루프린트 클래스에 애니메이션 블루프린트를 세팅해준다.
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		static ConstructorHelpers::FClassFinder<UAnimInstance> PlayerAnim(TEXT("/Game/BluePrints/AB_AnimBlueprint.AB_AnimBlueprint_C"));
		if (PlayerAnim.Succeeded())
		{
			GetMesh()->SetAnimInstanceClass(PlayerAnim.Class);
		}
		else
		{
			PRINT_ERROR_LOG(TEXT("Player Animation is NULL"));
		}

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

		InteractionZone = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionZone"));	// 감지할 오브젝트 생성
		InteractionZone->SetupAttachment(tpsCamComp);										// 카메라에 감지할 오브젝트를 자식으로 추가
		InteractionZone->SetBoxExtent(FVector(15.f, 15.f, 250.f));							// 감지할 범위 설정
		InteractionZone->SetRelativeLocation(FVector(250, 0, 0));							// 위치 설정
		InteractionZone->SetRelativeRotation(FRotator(0, -90, 90));							// 회전 설정
		InteractionZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);					// 물리 충돌은 안 하고, 오버랩 감지만 허용
		InteractionZone->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
		InteractionZone->SetGenerateOverlapEvents(true);

		//ai controller 세팅(만약 플레이어가 조종하지 않는 캐릭터라면 ai_controller의 지배를 받게 된다.)
		//AIControllerClass = APE_AIController::StaticClass();
		//AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

		HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthStat"));
		InventoryComponent = CreateDefaultSubobject<UPE_InventoryComponent>(TEXT("InventoryComponent"));
		
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("Player Skeletal Mesh is NULL"));
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> HUDInventory(TEXT("WidgetBlueprint'/Game/BluePrints/UI/WB_Inventory.WB_Inventory_C'"));
	if (HUDInventory.Succeeded())
	{
		InventoryWidgetClass = HUDInventory.Class;
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("HUDInventory is NULL"));
	}
}

// Called when the game starts or when spawned
void AProjectPlayer::BeginPlay()
{
	Super::BeginPlay();

	//EnhancedInputSystem에 imc_TPS등록
	//오류가 난다면 TPSProjec.Build.cs에 모듈에서 EnhancedInput을 추가해 주자.
	APlayerController* pc = Cast<APlayerController>(Controller); //현재 플레이어의 APlayerController를 가져온다.
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer()); //입력 서브시스템을 가져와서
		if (subsystem)
		{
			subsystem->AddMappingContext(imc_ProjectPlayer, 0); //입력 컨텍스트에 등록한다.
		}
		else
		{
			PRINT_ERROR_LOG(TEXT("PlayerController is NULL"));
		}
		

		if (InventoryWidgetClass)
		{
			InventoryWidget = CreateWidget<UPE_Inventory>(pc, InventoryWidgetClass);
			if (InventoryWidget)
			{
				InventoryWidget->AddToViewport();
				//InventoryWidget->InitInventory(InventoryComponent);
			}
			else
			{
				PRINT_ERROR_LOG(TEXT("InventoryWidget is Not Created"));
			}
		}
		else
		{
			PRINT_ERROR_LOG(TEXT("InventoryWidgetClass is NULL"));
		}
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("PlayerController is NULL"));
	}

	UpdateCharacterStats(1); //캐릭터 스탯 설정

	InteractionZone->OnComponentBeginOverlap.AddDynamic(this, &AProjectPlayer::OnItemOverlapBegin);				// 이벤트 바인딩 : 아이템 감지 범위에 아이템 콜리전이 충돌했을때 
	InteractionZone->OnComponentEndOverlap.AddDynamic(this, &AProjectPlayer::OnItemOverlapEnd);					// 이벤트 바인딩 : 충돌범위에서 아이템이 빠져나갔을때

	if (pc)
	{
		FInputModeGameOnly InputMode;
		pc->SetInputMode(InputMode); //게임에만 입력을 받도록 설정
		pc->bShowMouseCursor = false; //마우스 커서 숨김

		if (pc->IsLocalController())
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerController is valid and local."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerController is NOT valid or not local."));
		}
	}
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
		PlayerInput->BindAction(IA_Interact, ETriggerEvent::Triggered, this, &AProjectPlayer::Interact);
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

//TakeDamage 오버라이드
float AProjectPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return HealthComp->ApplyDamage(DamageAmount);
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
	SprintStart_Client();
}

void AProjectPlayer::SprintStart_Client_Implementation()
{
	if (GetCharacterStats())
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStats()->SprintSpeed;
}

void AProjectPlayer::SprintEnd_Server_Implementation()
{
	if (GetCharacterStats())
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStats()->WalkSpeed;
	SprintEnd_Client();
}

void AProjectPlayer::SprintEnd_Client_Implementation()
{
	if (GetCharacterStats())
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStats()->WalkSpeed;
}

// 이벤트 바인딩 : 아이템이 감지되었을때
void AProjectPlayer::OnItemOverlapBegin(
	UPrimitiveComponent* OverlappedComp,								// 오버랩 이벤트를 발생시킨 자기자신의 Collision Component
	AActor* OtherActor,													// 충돌한 상대 Actor
	UPrimitiveComponent* OtherComp,										// 상대 Actor의 Collision Component
	int32 OtherBodyIndex,												// Skeletal Mesh에서 사용
	bool bFromSweep,													// 이동중 Sweep으로 감지된 경우 true
	const FHitResult& SweepResult)										// Sweep일 때 유효한 Hit 정보 (충돌 지점 좌표)
{
	APE_BasePickup* OverlappedItem = Cast<APE_BasePickup>(OtherActor);	// 감지된 액터를 아이템으로 변환. PE_BasePickup클래스가 아닐경우 nullptr반환
	if (OverlappedItem)
	{// PE_BasePickup 클래스(또는 상속)인 경우
		FocusedItem = OverlappedItem;									// 플레이어가 바라보고있는 아이템을 저장
		//Item->GetMesh()->SetRenderCustomDepth(true);			
	}
}

// 이벤트 바인딩 : 아이템이 감지범위 밖으로 나갔을때
void AProjectPlayer::OnItemOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APE_BasePickup* OverlappedItem = Cast<APE_BasePickup>(OtherActor);
	if (OverlappedItem && OverlappedItem == FocusedItem)
	{
		//OverlappedItem->GetMesh()->SetRenderCustomDepth(false);	// 추후 외곽선 표시를 위함
		FocusedItem = nullptr;										// 바라보고있는 아이템 해제
	}
}

void AProjectPlayer::Interact()
{
	if (FocusedItem)
	{
		FocusedItem->Interact(this);
	}
	// 밑에 여러 작용 추가
}
