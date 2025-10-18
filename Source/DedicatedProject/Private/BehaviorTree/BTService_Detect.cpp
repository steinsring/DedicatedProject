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

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	//UWorld* World = ControllingPawn->GetWorld();
	//FVector Center = ControllingPawn->GetActorLocation();
	//float DetectRadius = 600.0f;

	//if (nullptr == World) return;
	//TArray<FOverlapResult> OverlapResults;
	//FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	////DetectRadius 범위 안에 있는 충돌체를 탐지하여 OverlapResults에 넣어주고 있으면 true 없으면 false 반환
	//bool bResult = World->OverlapMultiByChannel(
	//	OverlapResults,
	//	Center,
	//	FQuat::Identity,
	//	ECollisionChannel::ECC_GameTraceChannel2,
	//	FCollisionShape::MakeSphere(DetectRadius),
	//	CollisionQueryParam
	//);

	//FVector Forward = ControllingPawn->GetActorForwardVector();
	//float FOV = 90.0f;
	//float CasHalfFOV = FMath::Cos(FMath::DegreesToRadians(FOV * 0.5f));

	//if (bResult)
	//{
	//	for (auto const& OverlapResult : OverlapResults)
	//	{
	//		AProjectPlayer* DetectedPlayer = Cast<AProjectPlayer>(OverlapResult.GetActor());
	//		if (DetectedPlayer && DetectedPlayer->GetController()->IsPlayerController())
	//		{
	//			FVector DirToPlayer = (DetectedPlayer->GetActorLocation() - ControllingPawn->GetActorLocation()).GetSafeNormal();
	//			float Dot = FVector::DotProduct(Forward, DirToPlayer);

	//			if (Dot >= CasHalfFOV)
	//			{
	//				//시야각 안에 플레이어가 들어왔을 때
	//				OwnerComp.GetBlackboardComponent()->SetValueAsObject(APE_AIController::TargetKey, DetectedPlayer);
	//				//DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f); // 감지 성공 시 초록색
	//				DrawDebugCone(
	//					World, 
	//					ControllingPawn->GetActorLocation(), 
	//					Forward, 
	//					DetectRadius, 
	//					FMath::DegreesToRadians(FOV * 0.5f), 
	//					FMath::DegreesToRadians(FOV * 0.5f), 
	//					16, 
	//					FColor::Green, 
	//					false, 
	//					0.2f
	//				);

	//				//ai로부터 감지한 플레이어까지 파란색 라인을 그려
	//				DrawDebugPoint(World, DetectedPlayer->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
	//				DrawDebugLine(World, ControllingPawn->GetActorLocation(), DetectedPlayer->GetActorLocation(), FColor::Blue, false, 0.2f);
	//				return;
	//			}
	//		}
	//	}
	//}

	FVector Forward = ControllingPawn->GetActorForwardVector();
	FVector Center = ControllingPawn->GetActorLocation();
	UWorld* World = ControllingPawn->GetWorld();
	if (!World) return;

	FVector BoxExtent(300.0f, 300.0f, 100.0f);
	FQuat Rotation = ControllingPawn->GetActorQuat();

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);

	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center + Forward * BoxExtent.X,
		Rotation,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeBox(BoxExtent),
		CollisionQueryParam
	);

	if (bResult)
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			AProjectPlayer* DetectedPlayer = Cast<AProjectPlayer>(OverlapResult.GetActor());
			if (DetectedPlayer && DetectedPlayer->GetController()->IsPlayerController())
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(APE_AIController::TargetKey, DetectedPlayer);
				// 디버그 포인트와 라인 그리기
				DrawDebugPoint(World, DetectedPlayer->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
				DrawDebugLine(World, ControllingPawn->GetActorLocation(), DetectedPlayer->GetActorLocation(), FColor::Blue, false, 0.2f);
				return;
			}
		}
	}

	// 디버그 박스 표시
	DrawDebugBox(
		World,
		Center + Forward * BoxExtent.X,
		BoxExtent,
		Rotation,
		bResult ? FColor::Green : FColor::Red,
		false,
		0.2f
	);

	BB->ClearValue(APE_AIController::TargetKey);
	// 감지되는 것이 없을 경우엔 빨간색
	//DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
	//DrawDebugCone(
	//	World,
	//	ControllingPawn->GetActorLocation(),
	//	Forward,
	//	DetectRadius,
	//	FMath::DegreesToRadians(FOV * 0.5f),
	//	FMath::DegreesToRadians(FOV * 0.5f),
	//	16,
	//	FColor::Red,
	//	false,
	//	0.2f
	//);
}