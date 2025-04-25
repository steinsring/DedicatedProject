// Fill out your copyright notice in the Description page of Project Settings.


#include "PE_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName APE_AIController::HomePosKey(TEXT("HomePos"));
const FName APE_AIController::PatrolPosKey(TEXT("PatrolPos"));
const FName APE_AIController::TargetKey(TEXT("Target"));

APE_AIController::APE_AIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/AI/BB_PECharacter.BB_PECharacter"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/AI/BT_PECharacter.BT_PECharacter"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}

	BBComp = GetBlackboardComponent();
}

void APE_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (UseBlackboard(BBAsset, BBComp))
	{
		//AIController가 Pawn을 소유했을때, HomePos의 위치를 Pawn의 위치로 세팅해준다.
		BBComp->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
		if (!RunBehaviorTree(BTAsset))
		{
			//
		}
	}
}
