// Fill out your copyright notice in the Description page of Project Settings.


#include "PE_AIController.h"
//#include "BehaviorTree/BehaviorTree.h"
//#include "BehaviorTree/BlackboardData.h"
//#include "Blueprint/AIBlueprintHelperLibrary.h"

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
}

void APE_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	/*if (UseBlackboard(BBAsset, Blackboard))
	{
		if (!RunBehaviorTree(BTAsset))
		{

		}
	}*/
}
