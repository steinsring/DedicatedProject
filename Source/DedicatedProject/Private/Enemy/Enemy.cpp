// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"
#include "Enemy/PE_AIController.h"
#include "Enemy/Enemy_AnimInstance.h"

#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Enemy/PE_ToiletMechStats.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ProjectPlayer.h"
#include "Net/UnrealNetwork.h"

#include "DedicatedProject.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicateMovement(true);

	//SightHacking용 눈 이펙트 메쉬 컴포넌트 생성
	SightHackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SightHackMesh"));
	SightHackMesh->SetupAttachment(GetMesh(), TEXT("SightHack"));

	SightHackMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SightHackMesh->SetCastShadow(false);
	SightHackMesh->SetVisibility(true, true);

	SightHackMesh->SetRelativeScale3D(FVector(0.25f, 0.5f, 0.5f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(
		TEXT("/Engine/BasicShapes/Plane.Plane")
	);
	if (PlaneMesh.Succeeded())
	{
		PRINT_LOG(TEXT("PlaneMesh Load Success"));
		SightHackMesh->SetStaticMesh(PlaneMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MosaicMat(
		TEXT("/Game/Asset/UI/M_Mosaic.M_Mosaic") 
	);
	if (MosaicMat.Succeeded())
	{
		PRINT_LOG(TEXT("MosaicMat Load Success"));
		SightHackMesh->SetMaterial(0, MosaicMat.Object);
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if (EyeMaterialIndex >= 0 && GetMesh())
	{
		MosaicMID = GetMesh()->CreateDynamicMaterialInstance(EyeMaterialIndex);
	}

	//bSightHacked = true;
	UpdateSightHackeVisual();
}

void AEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemy, bIsWandering);
	DOREPLIFETIME(AEnemy, bIsGoingBack);
	DOREPLIFETIME(AEnemy, bIsGoingLeft);

	DOREPLIFETIME(AEnemy, bSightHacked);
}

void AEnemy::SetHitbox(ECollisionEnabled::Type CollisionEnabled, UCapsuleComponent* HitBox)
{
	if (CollisionEnabled == ECollisionEnabled::NoCollision)
		HitActors.Empty();

	if (HitBox)
	{
		HitBox->SetCollisionEnabled(CollisionEnabled);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HitBox is NULL"));
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	AProjectPlayer* Player = Cast<AProjectPlayer>(DamageCauser);
	if (Player)
	{
		float DamageMultiplier = Player->GetAttackPowerMultiplier();
		float CalculatedDamage = DamageAmount * DamageMultiplier;
		HealthComp->ApplyDamage(CalculatedDamage);

		APE_AIController* AICon = Cast<APE_AIController>(GetController());
		if (AICon)
		{
			PRINT_LOG(TEXT("AICon Exist"));
			UBlackboardComponent* BB = AICon->GetBlackboardComponent();
			if (BB)
			{
				PRINT_LOG(TEXT("BB Exist"));
				BB->SetValueAsObject(APE_AIController::TargetKey, Player);
				BB->SetValueAsObject(TEXT("PrevTarget"), Player);
			}
		}

		return CalculatedDamage;
	}

	return 0.0f;
}

void AEnemy::SetIsGoingBack(bool bNewState)
{
	if (bIsGoingBack == bNewState)
		return;

	bIsGoingBack = bNewState;
}

void AEnemy::SetIsGoingLeft(bool bNewState)
{
	if (bIsGoingLeft == bNewState)
		return;

	bIsGoingLeft = bNewState;
}

void AEnemy::OnRep_SightHacked()
{
	UpdateSightHackeVisual();
}

void AEnemy::UpdateSightHackeVisual()
{
	if (!SightHackMesh)
		return;

	SightHackMesh->SetVisibility(bSightHacked, true);
}

void AEnemy::SetSightHacked_Server_Implementation(bool bNewState)
{
	bSightHacked = bNewState;
	UpdateSightHackeVisual();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}