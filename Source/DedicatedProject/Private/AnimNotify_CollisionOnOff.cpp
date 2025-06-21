// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_CollisionOnOff.h"
#include "ToiletMech.h"

void UAnimNotify_CollisionOnOff::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AToiletMech* ToiletMech = Cast<AToiletMech>(MeshComp->GetOwner()))
	{
		bool bleft = (HandType == EHandType::Left);
		
		//if(bEnable) ToiletMech->
	}
}
