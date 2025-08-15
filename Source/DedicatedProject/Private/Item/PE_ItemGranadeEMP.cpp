// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PE_ItemGranadeEMP.h"

APE_ItemGranadeEMP::APE_ItemGranadeEMP()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("/Game/Asset/Item/Granade/Granade.Granade"));
	if (StaticMesh.Succeeded())
	{
		GetItemThrowableMesh()->SetStaticMesh(StaticMesh.Object);
	}
}


