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
	FVector2D MinCoordinate; 	// 노드의 좌하단 좌표
	UPROPERTY()
	FVector2D MaxCoordinate;	// 노드의 우상단 좌표
	UPROPERTY()
	FVector2D CenterCoordinate;	// 노드의 중앙 좌표

	// 자식이 있는 경우 (leaf가 아닌 노드)
	TSharedPtr<FBSPNode> LeftChild = NULL; // 노드의 좌하단 좌표
	TSharedPtr<FBSPNode> RightChild = NULL;

	// 자식이 없는 경우 이웃 노드를 모두 저장 (leaf 노드)
	TSharedPtr<FBSPNode> LeftNode = NULL;
	TSharedPtr<FBSPNode> RightNode = NULL;
	TSharedPtr<FBSPNode> TopNode = NULL;
	TSharedPtr<FBSPNode> BottomNode = NULL;
	TSharedPtr<FBSPNode> TopFloorNode = NULL;
	TSharedPtr<FBSPNode> BottomFloorNode = NULL;

	bool isSpawned = false;			// 현재 노드가 스폰되어 있는가

	// 현재 노드가 이웃노드와 반드시 이어야 하면 true, 잇지 않아도 되면 false
	bool isConnectLeft = false;
	bool isConnectRight = false;
	bool isConnectTop = false;
	bool isConnectBottom = false;

	bool IsLeaf() const { return !LeftChild && !RightChild; }
	bool Split(const TSharedPtr<FBSPNode> Node, const FVector2D MaxRoomSize);

};


