// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/SkillManagerComponent.h"
#include "Player/ProjectPlayer.h"

#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"

#include "Item/PE_BasePickup.h"
#include "Enemy/PE_AIController.h"
#include "Enemy/Enemy.h"

#include "Player/PE_PlayerState.h"
#include "DedicatedProject.h"


// Sets default values for this component's properties
USkillManagerComponent::USkillManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	CurrentAugmentSkill = E_Skills::None;
	CurrentOverrideSkill = E_Skills::None;

	SetIsReplicatedByDefault(true); // 컴포넌트가 기본적으로 복제되도록 설정
}

// Called when the game starts
void USkillManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Player = Cast<AProjectPlayer>(GetOwner());

	APE_PlayerState* PS = Player ? Cast<APE_PlayerState>(Player->GetPlayerState()) : nullptr;
	if(PS)
	{
		OnSkillPointFromPlayerState(PS->GetSkillPoint());
		OnSkillStateFromPlayerState(PS->GetSkills());
	}
}

bool USkillManagerComponent::UnlockSkill(E_Skills Skill)
{
	APE_PlayerState* PS = GetPEPlayerState();
	if (!PS)
	{
		PRINT_LOG(TEXT("UnlockSkill: PlayerState is null"));
		return false;
	}

	// PlayerState가 들고 있는 최신 스킬 배열 기준으로 체크
	const TArray<FPE_SkillDataTable>& SkillArray = PS->GetSkills();

	int32 SkillIndex = static_cast<int32>(Skill);
	if (!SkillArray.IsValidIndex(SkillIndex))
	{
		PRINT_LOG(TEXT("UnlockSkill: Invalid Skill Index %d, Num=%d"),
			SkillIndex, SkillArray.Num());
		return false;
	}

	const FPE_SkillDataTable& SkillData = SkillArray[SkillIndex];

	// 이미 언락된 스킬이면 실패
	if (SkillData.bIsUnlocked)
	{
		PRINT_LOG(TEXT("UnlockSkill: %s already unlocked"), *SkillData.SkillName.ToString());
		return false;
	}

	// 현재 스킬포인트는 PlayerState 기준으로 읽는다
	CurrentSkillPoint = PS->GetSkillPoint();
	if (CurrentSkillPoint < SkillData.Cost)
	{
		PRINT_LOG(TEXT("Not Enough Skill Points: %s (Have: %d, Need: %d)"),
			*SkillData.SkillName.ToString(), CurrentSkillPoint, SkillData.Cost);
		return false;
	}

	CurrentSkillPoint -= SkillData.Cost;
	PRINT_LOG(TEXT("UnlockSkill Local: Spend %d, New CurrentSkillPoint=%d"),
		SkillData.Cost, CurrentSkillPoint);
	// 여기까지 왔으면 "로컬 판단으로는 성공" → 서버에 요청 + true 반환
	PS->UnlockSkill_Server(Skill);

	return true;
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
	// 1) PlayerState 가져오기
	APE_PlayerState* PS = GetPEPlayerState();
	if (!PS)
	{
		PRINT_LOG(TEXT("IsSkillUnlocked: PlayerState is null"));
		return false;
	}

	// 2) PlayerState의 Skills 배열 기준으로 판정
	const TArray<FPE_SkillDataTable>& SkillArray = PS->GetSkills();

	int32 SkillIndex = static_cast<int32>(Skill);
	if (!SkillArray.IsValidIndex(SkillIndex))
	{
		PRINT_LOG(TEXT("IsSkillUnlocked: Invalid index %d, Num=%d"),
			SkillIndex, SkillArray.Num());
		return false;
	}

	return SkillArray[SkillIndex].bIsUnlocked;
}

void USkillManagerComponent::OnSkillPointFromPlayerState(int32 NewSkillPoint)
{
	CurrentSkillPoint = NewSkillPoint;
}

void USkillManagerComponent::OnSkillStateFromPlayerState(const TArray<FPE_SkillDataTable>& NewSkills)
{
	Skills = NewSkills;
}

//////////////////Augment Skill////////////////////

void USkillManagerComponent::UseAugmentSkill(E_Skills Skill)
{
	//쿨타임 중이면 사용 불가
	if (bIsAugmentSkillInCoolTime)
	{
		PRINT_LOG(TEXT("Augment Skill is in Cool Time"));
		return;
	}

	if (!GetWorld())
	{
		PRINT_LOG(TEXT("World is null"));
		return;
	}

	//서버가 아닌 경우
	if (GetOwnerRole() < ROLE_Authority)
	{
		UseAugmentSkill_Server(Skill);
	}
	//서버(호스트)인 경우
	else
	{
		UseAugmentSkill_Multicast(Skill);
	}

	StartSkillCoolTime(E_SkillType::Augment, Skill);
}

void USkillManagerComponent::UseAugmentSkill_Server_Implementation(E_Skills Skill)
{
	UseAugmentSkill_Multicast(Skill);
}

void USkillManagerComponent::UseAugmentSkill_Multicast_Implementation(E_Skills Skill)
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
	bIsAugmentSkillInCoolTime = true;

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
	float Multiplier = 3.0f;
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
	//쿨타임 중이면 사용 불가
	if (bIsOverrideSkillInCoolTime)
	{
		PRINT_LOG(TEXT("Override Skill is in Cool Time"));
		return;
	}

	if (!GetWorld())
	{
		PRINT_LOG(TEXT("World is null"));
		return;
	}

	UseOverrideSkill_Server(Skill);
	////서버가 아닌 경우
	//if (GetOwnerRole() < ROLE_Authority)
	//{
	//	UseOverrideSkill_Server(Skill); //서버에 요청
	//}
	////서버(호스트)인 경우
	//else
	//{
	//	UseOverrideSkill_Multicast(Skill); //서버(호스트)면 바로 실행
	//}

	StartSkillCoolTime(E_SkillType::Override, Skill);
}

void USkillManagerComponent::UseOverrideSkill_Server_Implementation(E_Skills Skill)
{
	//UseOverrideSkill_Multicast(Skill);
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
		if (Player->IsLocallyControlled())
			SeeThrough();
		break;
	default:
		break;
	}
}

void USkillManagerComponent::UseOverrideSkill_Multicast_Implementation(E_Skills Skill)
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
		if (Player->IsLocallyControlled())
			SeeThrough();
		break;
	default:
		break;
	}
}

void USkillManagerComponent::GetHitResultActor(float Distance)
{
	Player = Cast<AProjectPlayer>(GetOwner());
	if (Player == nullptr)
	{
		PRINT_LOG(TEXT("Player is null"));
		return;
	}
	UCameraComponent* PlayerCamera = Player->GetPlayerCamComp();
	if (PlayerCamera == nullptr)
	{
		PRINT_LOG(TEXT("PlayerCamera is null"));
		return;
	}

	//FVector Start = Player->GetActorLocation();
	FVector Start = PlayerCamera->GetComponentLocation();
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

	//FColor LineColor = bHit ? FColor::Red : FColor::Green;
	//DrawDebugLine(GetWorld(), Start, End, LineColor, false, 2.0f, 0, 1.0f);

	//로컬 클라에서만 하이라이트
	if (Player->IsLocallyControlled())
	{
		if (bHit)
		{
			SetTargetHighlight(HitResult.GetActor());
		}
		else
		{//히트하지 않았으면 하이라이트 제거
			SetTargetHighlight(nullptr);
		}
	}

	if (bHit)
	{
		PRINT_LOG(TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
		//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Yellow, false, 2.0f);
		HitActor = HitResult.GetActor();
	}
	else
	{
		HitActor = nullptr;
	}

	return;
}

void USkillManagerComponent::SetTargetHighlight(AActor* NewTarget)
{
	if (HighlightedActor && HighlightedActor != NewTarget)
	{
		//이전 하이라이트 제거
		TArray<UStaticMeshComponent*> StaticMeshComps;
		HighlightedActor->GetComponents<UStaticMeshComponent>(StaticMeshComps);
		for (UStaticMeshComponent* MeshComp : StaticMeshComps)
		{
			if (MeshComp)
			{
				MeshComp->SetRenderCustomDepth(false);
			}
		}

		TArray<USkeletalMeshComponent*> SkeletalMeshComps;
		HighlightedActor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComps);
		for (USkeletalMeshComponent* MeshComp : SkeletalMeshComps)
		{
			if (MeshComp)
			{
				MeshComp->SetRenderCustomDepth(false);
			}
		}
	}

	if (Cast<AEnemy>(NewTarget))
	{
		HighlightedActor = NewTarget;
		if (!HighlightedActor) return;

		//새로운 하이라이트 설정
		TArray<UStaticMeshComponent*> StaticMeshCompsNew;
		HighlightedActor->GetComponents<UStaticMeshComponent>(StaticMeshCompsNew);
		for (UStaticMeshComponent* MeshComp : StaticMeshCompsNew)
		{
			if (MeshComp)
			{
				MeshComp->SetRenderCustomDepth(true);
				MeshComp->SetCustomDepthStencilValue(1);
			}
		}

		TArray<USkeletalMeshComponent*> SkeletalMeshCompsNew;
		HighlightedActor->GetComponents<USkeletalMeshComponent>(SkeletalMeshCompsNew);
		for (USkeletalMeshComponent* MeshComp : SkeletalMeshCompsNew)
		{
			if (MeshComp)
			{
				MeshComp->SetRenderCustomDepth(true);
				MeshComp->SetCustomDepthStencilValue(1);
			}
		}
	}
}

void USkillManagerComponent::SightHacking()
{
	GetHitResultActor(1000.f);

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

	EnemyController->DisableDetect(SightHackingDuration);
	HitEnemy->SetSightHacked_Server(true);
	PRINT_LOG(TEXT("SightHacked Set True"));

	if (UWorld* World = GetWorld())
	{
		const float Duration = SightHackingDuration;
		
		TWeakObjectPtr<AEnemy> WeakHitEnemy(HitEnemy);
		TWeakObjectPtr<USkillManagerComponent> WeakThis(this);
		World->GetTimerManager().SetTimer
		(
			SightHackingTimerHandle, 
			FTimerDelegate::CreateLambda([WeakThis, WeakHitEnemy]()
			{
				if (!WeakThis.IsValid() || !WeakHitEnemy.IsValid()) return;
				
				AEnemy* HitEnemy = WeakHitEnemy.Get();
				HitEnemy->SetSightHacked_Server(false);
				PRINT_LOG(TEXT("SightHacked Set False"));
			}), 
			WeakThis->SightHackingDuration, 
			false
		);
	}
}

void USkillManagerComponent::Slow()
{
	PRINT_LOG(TEXT("Slow Activated"));
}

void USkillManagerComponent::ElectricShock()
{
	PRINT_LOG(TEXT("Electric Shock Activated"));

	GetHitResultActor(1000.f);
	if (!HitActor)
	{
		PRINT_LOG(TEXT("No Actor Hit"));
		return;
	}

	if (GetOwnerRole() == ROLE_Authority)
	{
		AEnemy* HitEnemy = Cast<AEnemy>(HitActor);
		if (!HitEnemy)
		{
			PRINT_LOG(TEXT("Hit Actor is not an Enemy"));
			return;
		}
		HitEnemy->ApplyStun(10.0f);
		UHealthComponent* EnemyHealthComp = HitEnemy->FindComponentByClass<UHealthComponent>();
		if (EnemyHealthComp)
		{
			EnemyHealthComp->ApplyDamage(ElectricShockDamage);
			PRINT_LOG(TEXT("Electric Shock Damage Applied: %f"), ElectricShockDamage);
		}
	}
}

void USkillManagerComponent::SeeThrough()
{
	PRINT_LOG(TEXT("See Through Activated"));
	DetectItems();

	if (UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		TWeakObjectPtr<USkillManagerComponent> WeakThis(this);
		World->GetTimerManager().SetTimer
		(
			TimerHandle, 
			FTimerDelegate::CreateLambda([WeakThis]()
			{
				if (!WeakThis.IsValid()) return;

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
			}), 
			WeakThis->SeeThroughDuration, 
			false
		);
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

	PRINT_LOG(TEXT("Detected Items Count: %d"), DetectedItems.Num());
}

////////////////////////////////////////////////////////////////////

void USkillManagerComponent::StartSkillCoolTime(E_SkillType SkillType, E_Skills Skill)
{
	if (!GetWorld())
	{
		PRINT_LOG(TEXT("World is null"));
		return;
	}

	PRINT_LOG(TEXT("Skill Cool Time Started"));
	SetCoolTimeOnOff(SkillType, true);

	int32 SkillIndex = static_cast<int32>(Skill);
	if (!Skills.IsValidIndex(SkillIndex))
	{
		PRINT_LOG(TEXT("Invalid Skill Index"));
		SetCoolTimeOnOff(SkillType, false);
		return;
	}
	float CoolTime = Skills[SkillIndex].CoolTime;
	PRINT_LOG(TEXT("Cool Time: %f"), CoolTime);
	if (CoolTime <= 0.f)
	{
		PRINT_LOG(TEXT("No Cool Time for this Skill"));
		SetCoolTimeOnOff(SkillType, false);
		return;
	}

	FTimerHandle TimerHandle = (SkillType == E_SkillType::Augment) ? AugmentSkillCoolTimeTimerHandle : OverrideSkillCoolTimeTimerHandle;
	TWeakObjectPtr<USkillManagerComponent> WeakThis(this);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [WeakThis, SkillType]()
	{
		if (!WeakThis.IsValid()) return;

		switch (SkillType)
		{
		case E_SkillType::Augment:
			WeakThis->bIsAugmentSkillInCoolTime = false;
			break;
		case E_SkillType::Override:
			WeakThis->bIsOverrideSkillInCoolTime = false;
			break;
		default:
			break;
		}
		PRINT_LOG(TEXT("Skill Cool Time Ended"));
	}, CoolTime, false);
}

void USkillManagerComponent::SetCoolTimeOnOff(E_SkillType SkillType, bool bIsInCoolTime)
{
	switch (SkillType)
	{
	case E_SkillType::Augment:
		bIsAugmentSkillInCoolTime = bIsInCoolTime;
		break;
	case E_SkillType::Override:
		bIsOverrideSkillInCoolTime = bIsInCoolTime;
		break;
	default:
		break;
	}
}

int32 USkillManagerComponent::GetSkillCost(E_Skills Skill) const
{
	APE_PlayerState* PS = GetPEPlayerState();
	if (!PS)
	{
		PRINT_LOG(TEXT("GetSkillCost: PlayerState is null"));
		return 0;
	}

	const TArray<FPE_SkillDataTable>& SkillArray = PS->GetSkills();

	int32 SkillIndex = static_cast<int32>(Skill);
	if (!SkillArray.IsValidIndex(SkillIndex))
	{
		PRINT_LOG(TEXT("GetSkillCost: Invalid index %d, Num=%d"),
			SkillIndex, SkillArray.Num());
		return 0;
	}

	return SkillArray[SkillIndex].Cost;
}

int32 USkillManagerComponent::GetSkillFuelCost(E_Skills Skill) const
{
	int32 SkillIndex = static_cast<int32>(Skill);
	if (Skills.IsValidIndex(SkillIndex))
	{
		return Skills[SkillIndex].FuelCost;
	}
	return 0;
}

APE_PlayerState* USkillManagerComponent::GetPEPlayerState() const
{
	AProjectPlayer* OwnerPlayer = Cast<AProjectPlayer>(GetOwner());
	if (!OwnerPlayer) return nullptr;

	return OwnerPlayer->GetPlayerState<APE_PlayerState>();
}