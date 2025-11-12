// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PE_RespawnBtn.h"
#include "Kismet/GameplayStatics.h"

#include "DedicatedProject.h"

void UPE_RespawnBtn::OnClick_RespawnBtn(APlayerState* DeadPlayerState, AProjectPlayer* TargetDummy)
{
	if (!DeadPlayerState)
	{
		PRINT_LOG(TEXT("TargetPlayerState is NULL"));
		//return;
	}
	if (!TargetDummy)
	{
		PRINT_LOG(TEXT("TargetDummy is NULL"));
		//return;
	}

	if (!DeadPlayerState || !TargetDummy)
	{
		return;
	}

	AProjectPlayer* TargetPlayerPawn = Cast<AProjectPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (TargetPlayerPawn)
	{
		TargetPlayerPawn->RequestRespawn_Server(DeadPlayerState, TargetDummy);

		if (ParentRespawnWidget)
		{
			ParentRespawnWidget->RemoveFromParent();
			APlayerController* MyPC = Cast<APlayerController>(TargetPlayerPawn->GetController());
			if (MyPC)
			{
				PRINT_LOG(TEXT("Setting Input Mode to Game Only"));
				MyPC->SetInputMode(FInputModeGameOnly());
				MyPC->bShowMouseCursor = false;
			}
		}
	}
}
