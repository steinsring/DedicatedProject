// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/PlayerAI_AnimInstance.h"
#include "Enemy/PlayerAI.h"
#include "Weapon/PE_WeaponProjectileComponent.h"
#include "DedicatedProject.h"

UPlayerAI_AnimInstance::UPlayerAI_AnimInstance()
{

}

void UPlayerAI_AnimInstance::AnimNotify_PlayerAIShoot()
{
	auto PlayerAI = Cast<APlayerAI>(TryGetPawnOwner());
	if (!PlayerAI) return;

	if (!PlayerAI->HasAuthority()) return;

	PlayerAI->GetWeaponProjectileComponent()->Fire_Enemy();
}
