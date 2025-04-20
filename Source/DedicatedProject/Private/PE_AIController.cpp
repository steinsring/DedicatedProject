// Fill out your copyright notice in the Description page of Project Settings.


#include "PE_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName APE_AIController::HomePosKey(TEXT("HomePos"));
const FName APE_AIController::PatrolPosKey(TEXT("PatrolPos"));

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
		BBComp->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
		if (!RunBehaviorTree(BTAsset))
		{
			//
		}
	}
}
