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
	UPROPERTY()
	FVector2D CenterCoordinate;

	TSharedPtr<FBSPNode> LeftChild = NULL; //스마트 포인터
	TSharedPtr<FBSPNode> RightChild = NULL;

	TSharedPtr<FBSPNode> LeftNode = NULL;
	TSharedPtr<FBSPNode> RightNode = NULL;
	TSharedPtr<FBSPNode> TopNode = NULL;
	TSharedPtr<FBSPNode> BottomNode = NULL;
	TSharedPtr<FBSPNode> TopFloorNode = NULL;
	TSharedPtr<FBSPNode> BottomFloorNode = NULL;

	bool isSpawned = false;
	bool isConnectLeft = false; // 현재 노드가 이웃노드와 반드시 이어야 하면 true, 잇지 않아도 되면 false
	bool isConnectRight = false;
	bool isConnectTop = false;
	bool isConnectBottom = false;

	bool IsLeaf() const { return !LeftChild && !RightChild; }
	bool Split(const TSharedPtr<FBSPNode> Node, const FVector2D MaxRoomSize);

};


