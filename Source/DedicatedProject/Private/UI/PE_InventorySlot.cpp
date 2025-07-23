// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PE_InventorySlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "DedicatedProject.h"


void UPE_InventorySlot::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPE_InventorySlot::SetIcon(UTexture2D* Icon)
{
	if (IsValid(IconImage))
	{
		//FSlateBrush Brush;
		//Brush.SetResourceObject(Icon);
		//Brush.ImageSize = FVector2D(1024.f, 1024.f);
		//Brush.DrawAs = ESlateBrushDrawType::Image;

		//IconImage->SetBrush(Brush);
		IconImage->SetBrushFromTexture(Icon, true);
		Number->SetText(FText::FromString(TEXT("1")));
		//IconImage->Brush.SetResourceObject(Icon);
		PRINT_LOG(TEXT("Set Item Icon Success"));
	}
	else
	{
		PRINT_LOG(TEXT("IconImage is NULL"));
	}
	/*
	UTexture2D* TestTex = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Asset/StarterBundle/ModularSciFiMats/Textures/Decals/Text/T_Square_0_M.T_Square_0_M"));
	if (IsValid(TestTex))
	{
		IconImage->SetBrushFromTexture(TestTex);
		PRINT_LOG(TEXT("debug is NULL"));
	}*/
}

