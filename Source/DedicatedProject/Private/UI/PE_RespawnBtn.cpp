// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PE_RespawnBtn.h"
#include "Kismet/GameplayStatics.h"

#include "DedicatedProject.h"

void UPE_RespawnBtn::OnClick_RespawnBtn(APlayerState* TargetPlayerState, AProjectPlayer* TargetDummy)
{
	if (!TargetPlayerState)
	{
		PRINT_LOG(TEXT("TargetPlayerState is NULL"));
		//return;
	}
	if (!TargetDummy)
	{
		PRINT_LOG(TEXT("TargetDummy is NULL"));
		//return;
	}

	if (!TargetPlayerState || !TargetDummy)
	{
		return;
	}

	AProjectPlayer* TargetPlayerPawn = Cast<AProjectPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (TargetPlayerPawn)
	{
		TargetPlayerPawn->RequestRespawn_Server(TargetPlayerState, TargetDummy);

		RemoveFromParent();
	}
}
