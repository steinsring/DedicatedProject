// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FBSPNode.generated.h"

USTRUCT(BlueprintType)
struct FBSPNode
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FVector2D MinCoordinate; // double값으로 이루어진 2D좌표 구조체
	UPROPERTY()
	FVector2D MaxCoordinate;

	TSharedPtr<FBSPNode> Left; //스마트 포인터
	TSharedPtr<FBSPNode> Right;

	bool IsLeaf() const { return !Left && !Right; }
	bool Split(const TSharedPtr<FBSPNode> Node, const FVector2D MaxRoomSize);
	//void CreateRoom(TSharedPtr<FBSPNode> Node);

};


