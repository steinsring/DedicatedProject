// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/SkillManagerComponent.h"
#include "Player/ProjectPlayer.h"

#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"

#include "Enemy/PE_AIController.h"
#include "Enemy/Enemy.h"

#include "DedicatedProject.h"


// Sets default values for this component's properties
USkillManagerComponent::USkillManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	CurrentAugmentSkill = E_Skills::None;
	CurrentOverrideSkill = E_Skills::None;

	Player = Cast<AProjectPlayer>(GetOwner());

	UDataTable* SkillDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/DataTable/DT_SkillDataTable.DT_SkillDataTable"));
	if (SkillDataTable)
	{
		TArray<FPE_SkillDataTable*> AllSkills;
		SkillDataTable->GetAllRows<FPE_SkillDataTable>(TEXT("SkillData"), AllSkills);
		for (auto* Skill : AllSkills)
		{
			Skills.Add(*Skill);
		}
	}

	SetIsReplicatedByDefault(true); // 컴포넌트가 기본적으로 복제되도록 설정
}


void USkillManagerComponent::UnlockSkill(E_Skills Skill)
{
	int32 SkillIndex = static_cast<int32>(Skill);
	Skills[SkillIndex].bIsUnlocked = true;
	PRINT_LOG(TEXT("Skill %s unlocked!"), *Skills[SkillIndex].SkillName.ToString());
}

void USkillManagerComponent::LockSkill(E_Skills Skill)
{
	int32 SkillIndex = static_cast<int32>(Skill);
	Skills[SkillIndex].bIsUnlocked = true;
	PRINT_LOG(TEXT("Skill %s unlocked!"), *Skills[SkillIndex].SkillName.ToString());
}

void USkillManagerComponent::SetAugmentSkill(E_Skills skill)
{
	CurrentAugmentSkill = skill;
}

void USkillManagerComponent::SetOverrideSkill(E_Skills skill)
{
	CurrentOverrideSkill = skill;
}

bool USkillManagerComponent::IsSkillUnlocked(E_Skills Skill) const
{
	int32 SkillIndex = static_cast<int32>(Skill);
	return Skills[SkillIndex].bIsUnlocked;
}

// Called when the game starts
void USkillManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

//////////////////Augment Skill////////////////////

void USkillManagerComponent::UseAugmentSkill(E_Skills Skill)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		UseAugmentSkill_Server(Skill);
	}
	else
	{
		HandleAugmentSkill(Skill);
	}
}

void USkillManagerComponent::UseAugmentSkill_Server_Implementation(E_Skills Skill)
{
	HandleAugmentSkill(Skill);
}

void USkillManagerComponent::HandleAugmentSkill(E_Skills Skill)
{
	switch (Skill)
	{
	case E_Skills::AttackUp:
		AttackUp();
		break;
	case E_Skills::DefenseUp:
		DefenseUp();
		break;
	case E_Skills::SpeedUp:
		SpeedUp();
		break;
	default:
		break;
	}
}

void USkillManagerComponent::ActivateAugmentSkill(E_Skills skill, float Multiplier)
{
	if (Player)
	{
		switch (skill)
		{
		case E_Skills::AttackUp:
			Player->SetAttackPowerMultiplier(Multiplier);
			Player->SetAttackPower(Multiplier);
			break;

		case E_Skills::DefenseUp:
			Player->SetDamageMultiplier(Multiplier);
			break;

		case E_Skills::SpeedUp:
			Player->SetSpeedMultiplier(Multiplier);
			break;

		default:
			break;
		}
	}
}


void USkillManagerComponent::AttackUp()
{
	float Multiplier = 1.5f;
	PRINT_LOG(TEXT("Attack Up Activated"));
	ActivateAugmentSkill(E_Skills::AttackUp, Multiplier);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Multiplier]()
	{
		if (Player)
		{
			Player->SetAttackPowerMultiplier(1.0f);
			Player->SetAttackPower(Player->GetAttackPower() / Multiplier);
			PRINT_LOG(TEXT("Attack Up Deactivated"));
		}
	}, AttackUpDuration, false);
}

void USkillManagerComponent::DefenseUp()
{
	PRINT_LOG(TEXT("Defense Up Activated"));
	float Multiplier = 0.5f; // 데미지 50% 감소
	ActivateAugmentSkill(E_Skills::DefenseUp, Multiplier);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Multiplier]()
	{
		if (Player)
		{
			Player->SetDamageMultiplier(1.0f);
			PRINT_LOG(TEXT("Defense Up Deactivated"));
		}
	}, DefenseUpDuration, false);
}

void USkillManagerComponent::SpeedUp()
{
	PRINT_LOG(TEXT("Speed Up Activated"));
	float Multiplier = 1.5f; // 이동속도 50% 증가
	ActivateAugmentSkill(E_Skills::SpeedUp, Multiplier);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Multiplier]()
	{
		if (Player)
		{
			Player->SetSpeedMultiplier(1.0f);
			PRINT_LOG(TEXT("Speed Up Deactivated"));
		}
	}, SpeedUpDuration, false);
}

//////////////////Override Skill////////////////////

void USkillManagerComponent::UseOverrideSkill(E_Skills Skill)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		UseOverrideSkill_Server(Skill);
	}
	else
	{
		HandleOverrideSkill(Skill);
	}
}

void USkillManagerComponent::UseOverrideSkill_Server_Implementation(E_Skills Skill)
{
	HandleOverrideSkill(Skill);
	UseOverrideSkill_Multicast(Skill);
}

void USkillManagerComponent::UseOverrideSkill_Multicast_Implementation(E_Skills Skill)
{
	switch (Skill)
	{
	case E_Skills::SightHacking:
		//몬스터 눈 부분에 지지직 거리는 이펙트
		break;

	case E_Skills::ElectricShock:
		//몬스터 몸 부분에 전기 이펙트
		break;

	default:
		break;
	}
}

void USkillManagerComponent::HandleOverrideSkill(E_Skills Skill)
{
	switch (Skill)
	{
	case E_Skills::SightHacking:
		SightHacking();
		break;
	case E_Skills::Slow:
		Slow();
		break;
	case E_Skills::ElectricShock:
		ElectricShock();
		break;
	case E_Skills::SeeThrough:
		SeeThrough_Client();
		break;
	default:
		break;
	}
}

void USkillManagerComponent::GetHitResultActor(float Distance)
{
	UCameraComponent* PlayerCamera = Player->GetPlayerCamComp();
	if (PlayerCamera == nullptr)
	{
		PRINT_LOG(TEXT("PlayerCamera is null"));
		return;
	}

	FVector Start = Player->GetActorLocation();
	FVector End = Start + (PlayerCamera->GetForwardVector() * Distance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Player);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Pawn,
		Params
	);

	FColor LineColor = bHit ? FColor::Red : FColor::Green;
	DrawDebugLine(GetWorld(), Start, End, LineColor, false, 2.0f, 0, 1.0f);

	if (bHit)
	{
		PRINT_LOG(TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Yellow, false, 2.0f);
		HitActor = HitResult.GetActor();
	}

	return;
}



void USkillManagerComponent::SightHacking()
{
	PRINT_LOG(TEXT("Sight Hacking Activated"));

	if (!HitActor)
	{
		PRINT_LOG(TEXT("No Actor Hit"));
		return;
	}

	AEnemy* HitEnemy = Cast<AEnemy>(HitActor);
	if (!HitEnemy)
	{
		PRINT_LOG(TEXT("Hit Actor is not an Enemy"));
		return;
	}

	APE_AIController* EnemyController = Cast<APE_AIController>(HitEnemy->GetController());
	if (!EnemyController)
	{
		PRINT_LOG(TEXT("EnemyController is null"));
		return;
	}

	EnemyController->DisableDetect(10.0f);
}

void USkillManagerComponent::Slow()
{
	PRINT_LOG(TEXT("Slow Activated"));
}

void USkillManagerComponent::ElectricShock()
{
	PRINT_LOG(TEXT("Electric Shock Activated"));
	//GetHitResultActor(1000.0f);

	if (!HitActor)
	{
		PRINT_LOG(TEXT("No Actor Hit"));
		return;
	}

	AEnemy* HitEnemy = Cast<AEnemy>(HitActor);
	if (!HitEnemy)
	{
		PRINT_LOG(TEXT("Hit Actor is not an Enemy"));
		return;
	}

	HitEnemy->ApplyStun(10.0f);
}

//See Through는 Client에서 실행
void USkillManagerComponent::SeeThrough_Client_Implementation()
{
	SeeThrough();
}

void USkillManagerComponent::SeeThrough()
{
	PRINT_LOG(TEXT("See Through Activated"));
	DetectItems();

	if (UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		TWeakObjectPtr<USkillManagerComponent> WeakThis(this);
		World->GetTimerManager().SetTimer(TimerHandle, [WeakThis]()
		{
			for (auto Item : WeakThis->DetectedItems)
			{
				if (Item)
				{
					TArray<UStaticMeshComponent*> MeshComps;
					Item->GetComponents<UStaticMeshComponent>(MeshComps);
					for (UStaticMeshComponent* MeshComp : MeshComps)
					{
						if (MeshComp)
						{
							MeshComp->SetRenderCustomDepth(false);
						}
					}
				}
			}
			PRINT_LOG(TEXT("See Through Deactivated"));
		}, WeakThis->SeeThroughDuration, false);
	}
}

void USkillManagerComponent::DetectItems()
{
	//배열 초기화
	DetectedItems.Empty();

	auto ControllingPawn = Cast<APawn>(Player);
	if (ControllingPawn == nullptr) return;

	// 1. NavMeshBoundsVolume 중 현재 캐릭터가 들어 있는 볼륨 찾기
	TArray<AActor*> NavVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANavMeshBoundsVolume::StaticClass(), NavVolumes);

	ANavMeshBoundsVolume* CurrentVolume = nullptr;
	const FVector PawnLocation = ControllingPawn->GetActorLocation();
	for (auto VolumeActor : NavVolumes)
	{
		ANavMeshBoundsVolume* Volume = Cast<ANavMeshBoundsVolume>(VolumeActor);
		if (nullptr == Volume) continue;

		FVector Origin, Extent;
		Volume->GetActorBounds(false, Origin, Extent);

		const FVector Delta = (PawnLocation - Origin).GetAbs();
		if (Delta.X <= Extent.X && Delta.Y <= Extent.Y && Delta.Z <= Extent.Z)
		{
			CurrentVolume = Volume;
			PRINT_LOG(TEXT("Volume Selected"));
			break;
		}
	}
	if (nullptr == CurrentVolume) return;

	// 2. 현재 NavMeshBoundsVolume 안에 있는 Item 찾기
	FVector VolumeOrigin, VolumeExtent;
	CurrentVolume->GetActorBounds(false, VolumeOrigin, VolumeExtent);

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
	for (AActor* CurActor : AllActors)
	{
		APE_BasePickup* Item = Cast<APE_BasePickup>(CurActor);
		if (nullptr == Item) continue;

		FVector Loc = Item->GetActorLocation();
		FVector Delta = (Loc - VolumeOrigin).GetAbs();
		if (Delta.X <= VolumeExtent.X && Delta.Y <= VolumeExtent.Y && Delta.Z <= VolumeExtent.Z)
		{
			DetectedItems.Add(Item);

			// 아이템 하이라이트 처리
			TArray<UStaticMeshComponent*> MeshComps;
			Item->GetComponents<UStaticMeshComponent>(MeshComps);
			for (UStaticMeshComponent* MeshComp : MeshComps)
			{
				if (MeshComp)
				{
					MeshComp->SetRenderCustomDepth(true);
					MeshComp->SetCustomDepthStencilValue(1); // SeeThrough용 Stencil 값
				}
			}
		}
	}
}

int32 USkillManagerComponent::GetSkillCost(E_Skills Skill) const
{
	int32 SkillIndex = static_cast<int32>(Skill);
	if (Skills.IsValidIndex(SkillIndex))
	{
		return Skills[SkillIndex].Cost;
	}
	return 0;
}
