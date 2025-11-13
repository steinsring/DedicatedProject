// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/ProjectPlayer.h"
#include "DedicatedProject.h"

#include "Engine/DataTable.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h" //EnhancedInput 사용을 위함
#include "InputAction.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "PE_GameMode.h"
#include "PE_GameState.h"
#include "Net/UnrealNetwork.h"
#include "UI/PE_RespawnWidget.h"
#include "GameFramework/GameStateBase.h"
#include "Player/PE_AnimInstance.h"

#include "Inventory/PE_InventoryComponent.h"
#include "Enemy/PE_AIController.h"
#include "Player/PE_CharacterStats.h"
#include "Player/PE_AnimInstance.h"
#include "UI/PE_Inventory.h"
#include <Camera/CameraComponent.h> // 카메라
#include <GameFramework/SpringArmComponent.h> //3인칭 카메라암
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Fuel/PE_FuelComponent.h"
#include <Components/SpotLightComponent.h>		// 조명
#include "Player/PE_ItemThrowableComponent.h"
#include "PE_Interactable.h"
#include "Player/PE_PlayerController.h"
#include "Player/PE_PlayerState.h"
#include "Weapon/PE_WeaponProjectileComponent.h"



// Sets default values
AProjectPlayer::AProjectPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicates(true);
	SetReplicateMovement(true);

	//스켈레탈 메쉬를 구조체로 불러와서
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Meshes/Wraith.Wraith"));
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
		static ConstructorHelpers::FClassFinder<UAnimInstance> PlayerAnimAsset(TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Wraith_AnimBlueprint.Wraith_AnimBlueprint_C"));
		if (PlayerAnimAsset.Succeeded())
		{
			GetMesh()->SetAnimInstanceClass(PlayerAnimAsset.Class);
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
		InteractionZone->SetBoxExtent(FVector(15.f, 15.f, 350.f));							// 감지할 범위 설정
		InteractionZone->SetRelativeLocation(FVector(250, 0, 0));							// 위치 설정
		InteractionZone->SetRelativeRotation(FRotator(0, -90, 90));							// 회전 설정
		InteractionZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);					// 물리 충돌은 안 하고, 오버랩 감지만 허용
		InteractionZone->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
		InteractionZone->SetGenerateOverlapEvents(true);

		//ai controller 세팅(만약 플레이어가 조종하지 않는 캐릭터라면 ai_controller의 지배를 받게 된다.)
		//AIControllerClass = APE_AIController::StaticClass();
		//AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

		//히트박스 세팅-------------------------------------------------------------------------------
		RightFootHitBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightFootHitBox"));
		RightFootHitBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("foot_r")); //메쉬의 발에 히트박스 추가
		RightFootHitBox->SetCapsuleHalfHeight(30.0f); //높이 설정
		RightFootHitBox->SetCapsuleRadius(20.0f); //반지름 설정

		// 던지는 아이템 -----------------------------------------------------------------------------
		ItemThrowable = CreateDefaultSubobject<UPE_ItemThrowableComponent>(TEXT("ItemThrowable"));
		ItemThrowable->SetupAttachment(RootComponent);
		ItemThrowable->SetRelativeLocation(FVector(4.f, -11.f, 80.f));

		// InventoryComponent 초기화 --------------------------------------------------------------------
		InventoryComponent = CreateDefaultSubobject<UPE_InventoryComponent>(TEXT("InventoryComponent"));
		InventoryComponent->SetItemThrowableComponent(ItemThrowable);

		// FuelComponent 세팅------------------------------------------------------------------------
		FuelComponent = CreateDefaultSubobject<UPE_FuelComponent>(TEXT("FuelComponent"));
		FuelComponent->SetupAttachment(GetMesh(), TEXT("FX_backpack"));					// BackPack소켓에 직접 부착

		// SpotLight 생성----------------------------------------------------------------------------
		SpotLightComp = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
		SpotLightComp->SetupAttachment(GetMesh(), TEXT("HealthBar"));
		SpotLightComp->SetRelativeLocation(FVector(0.f, 70.f, 0.f));
		SpotLightComp->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

		// WeponProjectile ----------------------------------------------------------------------------
		WeaponProjectileComponent = CreateDefaultSubobject<UPE_WeaponProjectileComponent>(TEXT("WeponProjectile"));
		WeaponProjectileComponent->SetupAttachment(GetMesh(), TEXT("HealthBar"));
		WeaponProjectileComponent->SetRelativeLocation(FVector(-26.f, 80.f, -10.f));
		WeaponProjectileComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("Player Skeletal Mesh is NULL"));
	}

	// 스킬 --------------------------------------------------------------------------
	CharacterDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/DataTable/DT_CharacterStats.DT_CharacterStats"));
	SkillManager = CreateDefaultSubobject<USkillManagerComponent>(TEXT("SkillManager"));

	if (SkillManager)
	{
		PRINT_LOG(TEXT("✅ SkillManagerComponent is Created"));
	}
	else
	{
		PRINT_LOG(TEXT("❌ SkillManagerComponent is NOT Created"));
	}

	// 플레이어 Input Action 초기화 --------------------------------------------------------------------------
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMCPlayer(TEXT("/Game/Inputs/IMC_ProjectPlayer.IMC_ProjectPlayer"));
	if (IMCPlayer.Succeeded())	imc_ProjectPlayer = IMCPlayer.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IALookUp(TEXT("/Game/Inputs/IA_LookUp.IA_LookUp"));
	if (IALookUp.Succeeded())	ia_LookUp = IALookUp.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IATurn(TEXT("/Game/Inputs/IA_Turn.IA_Turn"));
	if (IATurn.Succeeded())	ia_Turn = IATurn.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IAMove(TEXT("/Game/Inputs/IA_PlayerMove.IA_PlayerMove"));
	if (IAMove.Succeeded())	ia_Move = IAMove.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IAJump(TEXT("/Game/Inputs/IA_PlayerJump.IA_PlayerJump"));
	if (IAJump.Succeeded())	ia_Jump = IAJump.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IAAttack(TEXT("/Game/Inputs/IA_Attack.IA_Attack"));
	if (IAAttack.Succeeded())	ia_Attack = IAAttack.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IASprint(TEXT("/Game/Inputs/IA_Sprint.IA_Sprint"));
	if (IASprint.Succeeded())	ia_Sprint = IASprint.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IAInteract(TEXT("/Game/Inputs/IA_Interact.IA_Interact"));
	if (IAInteract.Succeeded())	IA_Interact = IAInteract.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IASkillAugment(TEXT("/Game/Inputs/IA_Skill_Augment.IA_Skill_Augment"));
	if (IASkillAugment.Succeeded())	ia_Skill_Augment = IASkillAugment.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IASkillOverride(TEXT("/Game/Inputs/IA_Skill_Override.IA_Skill_Override"));
	if (IASkillOverride.Succeeded())	ia_Skill_Override = IASkillOverride.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IAItemUse(TEXT("/Game/Inputs/IA_ItemUse.IA_ItemUse"));
	if (IAItemUse.Succeeded())	IA_ItemUse = IAItemUse.Object;
}

// Called when the game starts or when spawned
void AProjectPlayer::BeginPlay()
{
	Super::BeginPlay();

	PlayerAnim = Cast<UPE_AnimInstance>(GetMesh()->GetAnimInstance());
	if (PlayerAnim && PlayerAnim->BasicAttack)
	{
		PRINT_LOG(TEXT("✅ PlayerAnim 초기화 완료: BasicAttack is valid"));
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("❌ InitPlayerAnim: BasicAttack is NULL"));
	}

	FString OwnerName = GetName();
	PRINT_LOG(TEXT("My Character name : %s"), *OwnerName);
	
	UpdateCharacterStats(1); //캐릭터 스탯 설정

	InteractionZone->OnComponentBeginOverlap.AddDynamic(this, &AProjectPlayer::OnItemOverlapBegin);				// 이벤트 바인딩 : 아이템 감지 범위에 아이템 콜리전이 충돌했을때 
	InteractionZone->OnComponentEndOverlap.AddDynamic(this, &AProjectPlayer::OnItemOverlapEnd);					// 이벤트 바인딩 : 충돌범위에서 아이템이 빠져나갔을때

	//if (HasAuthority())
	//{
	//	if (GetController() == nullptr)
	//	{
	//		SetPowerState_Multicast(false);
	//	}
	//}
}

// 서버: Possess 직후
void AProjectPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

// 클라: 컨트롤러 복제 도착/변경시
void AProjectPlayer::OnRep_Controller()
{
	Super::OnRep_Controller();
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
	//EnhancedInputSystem에 imc_TPS등록
	//오류가 난다면 TPSProjec.Build.cs에 모듈에서 EnhancedInput을 추가해 주자.

	//MappingContext 초기화 -------------------------------------------------------------------
	FInputModeGameOnly InputMode;
	APE_PlayerController* MyPlayerController = Cast<APE_PlayerController>(Controller);
	MyPlayerController->SetInputMode(InputMode); //게임에만 입력을 받도록 설정
	MyPlayerController->bShowMouseCursor = false; //마우스 커서 숨김
	auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(MyPlayerController->GetLocalPlayer()); //입력 서브시스템을 가져와서
	if (subsystem)
	{
		subsystem->AddMappingContext(imc_ProjectPlayer, 0); //입력 컨텍스트에 등록한다.
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("subsystem is NULL"));
	}
	
	// InputAction 바인드 -------------------------------------------------------------------------
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
		PlayerInput->BindAction(IA_Interact, ETriggerEvent::Started, this, &AProjectPlayer::Interact);
		PlayerInput->BindAction(ia_Attack, ETriggerEvent::Started, this, &AProjectPlayer::InputAttack);
		PlayerInput->BindAction(ia_Skill_Augment, ETriggerEvent::Started, this, &AProjectPlayer::InputAugmentSkill);
		PlayerInput->BindAction(ia_Skill_Override, ETriggerEvent::Triggered, this, &AProjectPlayer::WhileHoldingOverrideSkill);
		PlayerInput->BindAction(ia_Skill_Override, ETriggerEvent::Completed, this, &AProjectPlayer::OnReleaseOverrideSkill);
		PlayerInput->BindAction(IA_ItemUse, ETriggerEvent::Started, this, &AProjectPlayer::ItemUse);
	}

	InventoryComponent->InitInputAction(MyPlayerController);
}

void AProjectPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	RightFootHitBox->OnComponentBeginOverlap.AddDynamic(this, &AProjectPlayer::OnHitboxOverlap); //히트박스 오버랩 이벤트 바인딩
}

//------------------------------------------리스폰 관련--------------------------------------------------------

void AProjectPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectPlayer, bIsPowerOn);
}

void AProjectPlayer::SetPowerState_Multicast_Implementation(bool bNewPowerState)
{
	bIsPowerOn = bNewPowerState;

	UPE_AnimInstance* Anim = Cast<UPE_AnimInstance>(GetMesh()->GetAnimInstance());
	if (!Anim)
	{
		PRINT_ERROR_LOG(TEXT("AnimInstance is NULL in SetPowerState_Multicast"));
		return;
	}

	if (bIsPowerOn)
	{
		//AnimInstance->bIsPowerOn = true;
		Anim->SetPowerState(true);

		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		EnableInput(Cast<APlayerController>(GetController()));
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}
	else
	{
		Anim->SetPowerState(false);
		PRINT_LOG(TEXT("Power Off - Disable Movement"));

		GetMesh()->GlobalAnimRateScale = 0.0f;
		GetCharacterMovement()->DisableMovement();
		DisableInput(Cast<APlayerController>(GetController()));
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	}
}

// 서버에 Respawn 요청
void AProjectPlayer::RequestRespawn_Server_Implementation(APlayerState* DeadPlayerState, AProjectPlayer* TargetDummy)
{
	APE_GameMode* GM = GetWorld()->GetAuthGameMode<APE_GameMode>();
	if (GM)
	{
		GM->HandlePlayerRespawn(DeadPlayerState, TargetDummy);
	}
}

void AProjectPlayer::OpenRespawnUI(AProjectPlayer* TargetDummy)
{
	PRINT_LOG(TEXT("OpenRespawnUI Called"));
	if (!IsLocallyControlled())
	{
		PRINT_LOG(TEXT("Not Locally Controlled"));
		return;
	}

	if (TargetDummy->bIsPowerOn)
	{
		PRINT_LOG(TEXT("TargetDummy is already active"));
		return;
	}
	
	PRINT_LOG(TEXT("OpenRespawnUI Proceeded"));
    TSubclassOf<UPE_RespawnWidget> WidgetClass = LoadClass<UPE_RespawnWidget>
		(nullptr, TEXT("/Game/BluePrints/UI/WB_RespawnWidget.WB_RespawnWidget_C"));

	if (WidgetClass)
	{
		PRINT_LOG(TEXT("WidgetClass Loaded"));
		UPE_RespawnWidget* RespawnWidget = CreateWidget<UPE_RespawnWidget>(GetWorld(), WidgetClass);
		if (RespawnWidget)
		{
			PRINT_LOG(TEXT("RespawnWidget Created"));
			RespawnWidget->SetTargetDummy(TargetDummy);
			RespawnWidget->AddToViewport();
			
			if (!TargetDummy)
			{
				PRINT_LOG(TEXT("TargetDummy is NULL"));
			}
			//RespawnWidget->SetTargetPlayerState(DeadPlayerState);
			//if (!DeadPlayerState)
			//{
			//	PRINT_LOG(TEXT("TargetDummy's PlayerState is NULL"));
			//}

			APlayerController* MyPC = Cast<APlayerController>(GetController());
			if (MyPC)
			{
				PRINT_LOG(TEXT("Setting Input Mode to UI Only"));
				MyPC->SetInputMode(FInputModeUIOnly());
				MyPC->bShowMouseCursor = true;
			}
		}
	}
}
//----------------------------------------------------------------------------------------------------------

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

void AProjectPlayer::InputAttack(const FInputActionValue& inputValue)  
{  
   PRINT_LOG(TEXT("InputAttack Called"));  
   if (!PlayerAnim)  
   {  
       PRINT_ERROR_LOG(TEXT("PlayerAnim is NULL"));  
       return;  
   }  

   if (!WeaponProjectileComponent)
   {
	   PRINT_ERROR_LOG(TEXT("WeaponProjectileComponent is NULL"));
	   return;
   }

   WeaponProjectileComponent->Fire();
   
   if (PlayerAnim->BasicAttack)  
   {  
       UAnimMontage* AttackMontage = Cast<UAnimMontage>(PlayerAnim->BasicAttack);  
       if (AttackMontage)  
       {  
           ACharacterCommon::Attack(AttackMontage);  
       }  
       else  
       {  
           PRINT_ERROR_LOG(TEXT("BasicAttack is not a UAnimMontage"));  
       }  
   }  
   else  
   {  
       PRINT_ERROR_LOG(TEXT("BasicAttack is NULL"));  
   }
}

void AProjectPlayer::ItemUse(const FInputActionValue& inputValue)
{
	InventoryComponent->UseItem(this);
	PRINT_LOG(TEXT("UseItem"));
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
	AttackPower = GetCharacterStats()->AttackPower; //공격력 업데이트
}

void AProjectPlayer::Interact(AActor* Interactor)
{
	PRINT_LOG(TEXT("AProjectPlayer Interact Called by %s"), *Interactor->GetName());
}

//TakeDamage 오버라이드
float AProjectPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float CalculatedDamage = DamageAmount * DamageMultiplier;
	HealthComp->ApplyDamage(CalculatedDamage);

	return CalculatedDamage;
}

//void AProjectPlayer::Die_Server()
//{
//}
//
//void AProjectPlayer::Die_Multicast()
//{
//}

void AProjectPlayer::Die()
{
	if (HasAuthority())
	{
		APE_GameMode* GM = GetWorld()->GetAuthGameMode<APE_GameMode>();
		if (GM)
		{
			GM->HandlePlayerDeath(Controller);
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
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStats()->SprintSpeed * SpeedMultiplier;
	SprintStart_Client();
}

void AProjectPlayer::SprintStart_Client_Implementation()
{
	if (GetCharacterStats())
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStats()->SprintSpeed * SpeedMultiplier;
}

void AProjectPlayer::SprintEnd_Server_Implementation()
{
	if (GetCharacterStats())
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStats()->WalkSpeed * SpeedMultiplier;
	SprintEnd_Client();
}

void AProjectPlayer::SprintEnd_Client_Implementation()
{
	if (GetCharacterStats())
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterStats()->WalkSpeed * SpeedMultiplier;
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
	if (OtherActor && OtherActor->GetClass()->ImplementsInterface(UPE_Interactable::StaticClass()))
    {
        // FocusedItem 채우기: UObject* + Interface* 둘 다 세팅
        FocusedItem.SetObject(OtherActor);
        FocusedItem.SetInterface(Cast<IPE_Interactable>(OtherActor));

        // 외곽선 등 효과를 주고 싶으면 GetObject()로 액터를 꺼내서 처리
    }
}

// 이벤트 바인딩 : 아이템이 감지범위 밖으로 나갔을때
void AProjectPlayer::OnItemOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && FocusedItem && FocusedItem.GetObject() == OtherActor)
	{
		FocusedItem = nullptr; // 비우기
	}
}

void AProjectPlayer::Interact()
{
	PRINT_LOG(TEXT("Interact Called"));
	AProjectPlayer* DummyPlayer = Cast<AProjectPlayer>(FocusedItem.GetObject());
	if (DummyPlayer != nullptr)
	{
		OpenRespawnUI(DummyPlayer);
		PRINT_LOG(TEXT("Interacting with DummyPlayer"));
		return;
	}
	if (FocusedItem)
	{
		FocusedItem->Interact(this);
	}
	// 밑에 여러 작용 추가
}

void AProjectPlayer::InputAugmentSkill(const FInputActionValue& inputValue)
{
	//PRINT_LOG(TEXT("%s is activated"), *SkillManager->GetCurrentAugmentSkillName());
	E_Skills CurrentAugmentSkill = SkillManager->GetCurrentAugmentSkill();
	int32 cost = SkillManager->GetSkillCost(CurrentAugmentSkill);

	if (FuelComponent->HasEnoughFuel(cost) && !SkillManager->bIsAugmentSkillInCoolTime)
	{
		FuelComponent->UseFuel(cost);
		SkillManager->UseAugmentSkill(CurrentAugmentSkill);
	}
	else
	{
		PRINT_LOG(TEXT("Not Enough Fuel Or In CoolTime"));
	}
}

void AProjectPlayer::WhileHoldingOverrideSkill(const FInputActionValue& inputValue)
{
	E_Skills CurrentOverrideSkill = SkillManager->GetCurrentOverrideSkill();

	switch (CurrentOverrideSkill)
	{
	default: //(SightHacking, ElectricShock)
		SkillManager->GetHitResultActor(1000.f);
		break;

	case E_Skills::SeeThrough:
		//SkillManager->SeeThrough();
		break;

	case E_Skills::None:
		//PRINT_LOG(TEXT("No Override Skill is selected"));
		break;
	
	}
}

void AProjectPlayer::OnReleaseOverrideSkill(const FInputActionValue& inputValue)
{
	E_Skills CurrentOverrideSkill = SkillManager->GetCurrentOverrideSkill();
	int32 cost = SkillManager->GetSkillCost(CurrentOverrideSkill);

	if (FuelComponent->HasEnoughFuel(cost) && !SkillManager->bIsOverrideSkillInCoolTime)
	{
		FuelComponent->UseFuel(cost);
		SkillManager->UseOverrideSkill(CurrentOverrideSkill);
	}
	else
	{
		if (!FuelComponent->HasEnoughFuel(cost))
		{
			PRINT_LOG(TEXT("Not Enough Fuel"));
		}
		else
		{
			PRINT_LOG(TEXT("In CoolTime"));
		}
	}
}

void AProjectPlayer::AddItemToInventory(FName ID, int32 Quantity)
{
	if (ID == "Fuel")
	{
		FuelComponent->AddFuel(ID);
	}
	else
	{
		InventoryComponent->AddItem(ID, Quantity, this);
	}
}