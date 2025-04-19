// Fill out your copyright notice in the Description page of Project Settings.

#include "FBSPNode.h"

bool FBSPNode::Split(const TSharedPtr<FBSPNode> Node, const FVector2D MaxRoomSize) // 현재 노드 , 최대 방 크기
{
    FVector2D Size = FVector2D(Node->MaxCoordinate - Node->MinCoordinate); // 현재 노드의 크기

    if (Size <= MaxRoomSize || !Node.IsValid()) // 최대 방 크기보다 작거나 Node가 Null인 경우 분할 종료
        return false;

    bool bSplitHorizontally = Size.X < Size.Y;  

    // 최소비율과 최소크기 조건 확인
    float Ratio = FMath::FRandRange(0.3f, 0.7f); // 30% ~ 70% 사이 분할
    //float Ratio = 0.5f; // 실험용 반만 분할

    if (bSplitHorizontally)
    {// 가로분할
        int SplitY = Node->MinCoordinate.Y + Size.Y * Ratio;
        Node->Left = MakeShared<FBSPNode>(FBSPNode{ Node->MinCoordinate, FVector2D(Node->MaxCoordinate.X, SplitY) });
        Node->Right = MakeShared<FBSPNode>(FBSPNode{ FVector2D(Node->MinCoordinate.X, SplitY), Node->MaxCoordinate  });
    }
    else
    { // 세로분할
        int SplitX = Node->MinCoordinate.X + Size.X * Ratio;
        Node->Left = MakeShared<FBSPNode>(FBSPNode{ Node->MinCoordinate, FVector2D(SplitX, Node->MaxCoordinate.Y) });
        Node->Right = MakeShared<FBSPNode>(FBSPNode{ FVector2D(SplitX, Node->MinCoordinate.Y), Node->MaxCoordinate });
    }
    //PRINT_LOG(TEXT("My Log : %s %s, %s %s"), TEXT("min : "), *Node->MinCoordinate.ToString(), TEXT("max : "), *Node->MaxCoordinate.ToString());

    //양쪽 자식노드 분할
    bool bLeft = Split(Node->Left, MaxRoomSize);
    bool bRight = Split(Node->Right, MaxRoomSize);

    return bLeft && bRight;
}

/*
void CreateRoom(TSharedPtr<FBSPNode> Node)
{
    if (!Node->IsLeaf()) return;

    FVector2D Size = FVector2D(Node->MaxCoordinate - Node->MinCoordinate);
    float Margin = 0.1f;

    FVector2D RoomMin = FVector2D(Node->MinCoordinate) + Size * Margin;
    FVector2D RoomMax = FVector2D(Node->MaxCoordinate) - Size * Margin;

    Node->RoomMin = RoomMin.IntPoint();
    Node->RoomMax = RoomMax.IntPoint();
}
*/
