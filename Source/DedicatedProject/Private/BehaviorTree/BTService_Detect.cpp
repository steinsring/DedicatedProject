// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTService_Detect.h"
#include "Enemy/PE_AIController.h"
#include "Player/ProjectPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	//호출하는 주기 설정
	Interval = 1.0f;
}

//BT의 노드에 Detect라는 이름의 서비스 추가(서비스 목록에 Detect라는 서비스 생김)
void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (nullptr == BB) return;

	bool bCanDetect = BB->GetValueAsBool("bCanDetect");
	if (!bCanDetect) return;

	APawn* ControllingPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!ControllingPawn) return;

	AActor* CurrentTarget = Cast<AActor>(BB->GetValueAsObject(APE_AIController::TargetKey));
	if (CurrentTarget)
	{
		float Distance = FVector::Dist(ControllingPawn->GetActorLocation(), CurrentTarget->GetActorLocation());

		if (Distance > LoseTargetDistance)
		{
			BB->ClearValue(APE_AIController::TargetKey);
		}
		else
		{
			return;
		}
	}

	UWorld* World = ControllingPawn->GetWorld();
	if (!World) return;

	FVector Forward = ControllingPawn->GetActorForwardVector();
	FVector Center = ControllingPawn->GetActorLocation();

	FVector BoxExtent(500.0f, 300.0f, 100.0f);
	FQuat Rotation = ControllingPawn->GetActorQuat();

	TArray<FOverlapResult> BoxOverlapResults;
	FCollisionQueryParams BoxCollisionQueryParam(NAME_None, false, ControllingPawn);

	bool bBoxResult = World->OverlapMultiByChannel(
		BoxOverlapResults,
		Center + Forward * BoxExtent.X,
		Rotation,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeBox(BoxExtent),
		BoxCollisionQueryParam
	);

	if (bBoxResult)
	{
		for (auto const& OverlapResult : BoxOverlapResults)
		{
			AProjectPlayer* DetectedPlayer = Cast<AProjectPlayer>(OverlapResult.GetActor());
			if (!DetectedPlayer) continue;
			AController* PlayerController = DetectedPlayer->GetController();
			if (!PlayerController) continue;
			if (PlayerController->IsPlayerController())
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(APE_AIController::TargetKey, DetectedPlayer);
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("PrevTarget"), DetectedPlayer);
				// 디버그 포인트와 라인 그리기
				//DrawDebugPoint(World, DetectedPlayer->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
				//DrawDebugLine(World, ControllingPawn->GetActorLocation(), DetectedPlayer->GetActorLocation(), FColor::Blue, false, 0.2f);
				return;
				//break;
			}
		}
	}

	// 디버그 박스 표시
	//DrawDebugBox(
	//	World,
	//	Center + Forward * BoxExtent.X,
	//	BoxExtent,
	//	Rotation,
	//	bBoxResult ? FColor::Green : FColor::Red,
	//	false,
	//	0.2f
	//);

	//BB->ClearValue(APE_AIController::TargetKey);
}