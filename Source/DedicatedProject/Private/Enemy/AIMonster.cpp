// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AIMonster.h"


// Sets default values
AAIMonster::AAIMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAIMonster::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAIMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

