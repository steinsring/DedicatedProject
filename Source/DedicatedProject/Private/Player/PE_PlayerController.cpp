// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PE_PlayerController.h"

APE_PlayerController::APE_PlayerController()
{
	SkillManager = CreateDefaultSubobject<USkillManagerComponent>(TEXT("SkillManager"));
}
