// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/ToiletMechTest.h"
#include "Enemy/PE_AIController.h"

#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"

#include "Enemy/PE_ToiletMechStats.h"
#include "Kismet/GameplayStatics.h"

#include "DedicatedProject.h"

//생성자에서 기본 애셋들 세팅해주기
AToiletMechTest::AToiletMechTest()
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
		static ConstructorHelpers::FClassFinder<UAnimInstance> MechAnim(TEXT("/Game/BluePrints/Enemy/AB_ToiletAnimBlueprint.AB_ToiletAnimBlueprint_C"));
		if (MechAnim.Succeeded())
		{
			GetMesh()->SetAnimInstanceClass(MechAnim.Class);
		}
		//이렇게 하면 블루프린트에 오류가 생겨 삭제하고 재생성했을때 자동으로 설정해준다.

		//ai controller 세팅(만약 플레이어가 조종하지 않는 캐릭터라면 ai_controller의 지배를 받게 된다.)
		AIControllerClass = APE_AIController::StaticClass();
		AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

		//히트박스 세팅
		LeftHandHitBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftHandHitbox"));
		RightHandHitBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightHandHitbox"));

		LeftHandHitBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("hand_l"));
		RightHandHitBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("hand_r"));

		//왼손
		LeftHandHitBox->SetCapsuleHalfHeight(30.0f);
		LeftHandHitBox->SetCapsuleRadius(20.0f);
		LeftHandHitBox->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

		//오른손
		RightHandHitBox->SetCapsuleHalfHeight(56.0f);
		RightHandHitBox->SetCapsuleRadius(20.0f);
		RightHandHitBox->SetRelativeLocation(FVector(-15.0f, -6.0f, 0.0f));
		RightHandHitBox->SetRelativeRotation(FRotator(0.0f, 83.0f, 16.0f));

		//UE_LOG(LogTemp, Warning, TEXT("ToiletMechTest: Hitboxes initialized."));

		//공격을 안할 처음에는 콜리전을 꺼준다.(공격 실행시 켜주고 공격 끝나면 꺼주어야 함)
		LeftHandHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RightHandHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	//데이터 테이블을 불러오기
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableAsset(TEXT
					("'/Game/DataTable/DT_ToiletMechStats.DT_ToiletMechStats'"));
	if (DataTableAsset.Succeeded())
		DataTable = DataTableAsset.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> HitSoundObject
	(TEXT("/Game/Asset/Sci-fi_UI_Pack/Audio/FX_Sounds/Sci-fi_UI_Pack_Notificationv.Sci-fi_UI_Pack_Notificationv"));
	if (HitSoundObject.Succeeded())
	{
		HitSound = HitSoundObject.Object;
	}
	else
	{
		PRINT_ERROR_LOG(TEXT("ToiletMech HitSound is NULL"));
	}
}

void AToiletMechTest::BeginPlay()
{
	Super::BeginPlay();
	
	//데이터 테이블에서 AttackPower 가져오기
	if (DataTable)
	{
		//디버깅용
		static const FString ContextString(TEXT("ToiletMech Stats Lookup"));

		FPE_ToiletMechStats* StatsRow = DataTable->FindRow<FPE_ToiletMechStats>(FName("Default"), ContextString);

		if (StatsRow)
		{
			AttackPower = StatsRow->AttackPower;
			MaxSpeed = StatsRow->MaxSpeed;
			Speed = StatsRow->Speed;
			UE_LOG(LogTemp, Warning, TEXT("Row 'Default' found in EnemyDataTable"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Row 'Default' not found in EnemyDataTable"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyDataTable is null"));
	}
}

//히트박스에 바인딩해주기
void AToiletMechTest::PostInitializeComponents()  
{  
   Super::PostInitializeComponents();  

   LeftHandHitBox->OnComponentBeginOverlap.AddDynamic(this, &AToiletMechTest::OnHitboxOverlap);  
   RightHandHitBox->OnComponentBeginOverlap.AddDynamic(this, &AToiletMechTest::OnHitboxOverlap);  
}