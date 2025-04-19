// Fill out your copyright notice in the Description page of Project Settings.


#include "PE_AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"

UPE_AnimInstance::UPE_AnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
}

//Tick마다 호출되는 AnimInstance함수
void UPE_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	//pawn을 상속받는 character가 불러와짐(ProjectPlayer)
	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		//UE_LOG(LogTemp, Log, TEXT("Speed : %f"), CurrentPawnSpeed);
		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}
}
