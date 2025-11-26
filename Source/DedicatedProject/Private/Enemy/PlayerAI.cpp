// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/PlayerAI.h"

#include "Enemy/ToiletMechTest.h"
#include "Enemy/PE_AIController.h"

#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Weapon/PE_WeaponProjectileComponent.h"

#include "Enemy/PE_ToiletMechStats.h"
#include "Kismet/GameplayStatics.h"

#include "DedicatedProject.h"

APlayerAI::APlayerAI()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;

	//스켈레탈 메쉬를 구조체로 불러와서
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Meshes/Wraith.Wraith"));
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
		static ConstructorHelpers::FClassFinder<UAnimInstance> PlayerAIAnim(TEXT("/Game/BluePrints/Enemy/AB_PlayerAIAnimBlueprint.AB_PlayerAIAnimBlueprint_C"));
		if (PlayerAIAnim.Succeeded())
		{
			GetMesh()->SetAnimInstanceClass(PlayerAIAnim.Class);
		}
		//이렇게 하면 블루프린트에 오류가 생겨 삭제하고 재생성했을때 자동으로 설정해준다.

		//ai controller 세팅(만약 플레이어가 조종하지 않는 캐릭터라면 ai_controller의 지배를 받게 된다.)
		AIControllerClass = APE_AIController::StaticClass();
		AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

		// WeponProjectile ----------------------------------------------------------------------------
		WeaponProjectileComponent = CreateDefaultSubobject<UPE_WeaponProjectileComponent>(TEXT("WeponProjectile"));
		//WeaponProjectileComponent->SetupAttachment(GetMesh(), TEXT("HealthBar"));
		WeaponProjectileComponent->SetupAttachment(RootComponent);
		WeaponProjectileComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
		WeaponProjectileComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	}

	//모자이크 위치 및 회전 조정
	SightHackMesh->SetRelativeRotation(FRotator(90.f, 90.f, 0.f));
	SightHackMesh->SetRelativeLocation(FVector(0.f, 50.f, 150.f));

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

void APlayerAI::BeginPlay()
{
	Super::BeginPlay();

	//데이터 테이블에서 AttackPower 가져오기
	if (DataTable)
	{
		//디버깅용
		static const FString ContextString(TEXT("PlayerAI Stats Lookup"));

		FPE_ToiletMechStats* StatsRow = DataTable->FindRow<FPE_ToiletMechStats>(FName("Default"), ContextString);

		if (StatsRow)
		{
			AttackPower = StatsRow->AttackPower;
			MaxSpeed = StatsRow->MaxSpeed;
			Speed = StatsRow->Speed;
			MaxHP = StatsRow->MaxHP;
			UE_LOG(LogTemp, Warning, TEXT("Row 'Default' found in EnemyDataTable"));

			if (HealthComp)
			{
				HealthComp->InitHealth(MaxHP);
			}
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