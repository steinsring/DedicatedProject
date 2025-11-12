// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PE_RespawnWidget.h"
#include "PE_GameState.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "UI/PE_RespawnBtn.h"

#include "DedicatedProject.h"


void UPE_RespawnWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeRespawnList();
}

void UPE_RespawnWidget::InitializeRespawnList()
{
	if (!RespawnListBox || !RespawnBtnClass)
	{
		PRINT_LOG(TEXT("RespawnListBox or RespawnBtnClass is NULL"));
		//return;
	}

	auto GS = Cast<APE_GameState>(UGameplayStatics::GetGameState(this));
	if (!GS)
	{
		PRINT_LOG(TEXT("GameState is NULL"));
		return;
	}

	RespawnListBox->ClearChildren();

	for (auto* DeadPS : GS->DeadPlayerStates)
	{
		if (!DeadPS)
		{
			PRINT_LOG(TEXT("Dead PlayerState is NULL"));
			continue;
		}

		UPE_RespawnBtn* NewBtn = CreateWidget<UPE_RespawnBtn>(this, RespawnBtnClass);
		if (NewBtn)
		{
			PRINT_LOG(TEXT("Respawn Button Created"));
			NewBtn->RespawnPlayerState = DeadPS;
			if (!DeadPS)
			{
				PRINT_LOG(TEXT("Dead PlayerState's PlayerState is NULL"));
			}
			NewBtn->RespawnTargetDummy = TargetDummy;
			if (!TargetDummy)
			{
				PRINT_LOG(TEXT("TargetDummy is NULL"));
			}
			NewBtn->ParentRespawnWidget = this;
			RespawnListBox->AddChild(NewBtn);
		}
	}
}

void UPE_RespawnWidget::SetTargetDummy(AProjectPlayer* InTargetDummy)
{
	TargetDummy = InTargetDummy;
	PRINT_LOG(TEXT("TargetDummy Set in RespawnWidget"));
}
