// Fill out your copyright notice in the Description page of Project Settings.

#include "Map/FBSPNode.h"
#include "DedicatedProject.h"

bool FBSPNode::Split(const TSharedPtr<FBSPNode> Node, const FVector2D MaxRoomSize) // 현재 노드 , 최대 방 크기
{
    if (!Node.IsValid()) return false;
    FVector2D Size = FVector2D(Node->MaxCoordinate - Node->MinCoordinate); // 현재 노드의 크기

    if (Size <= MaxRoomSize) // 최대 방 크기보다 작거나 Node가 Null인 경우 분할 종료
        return false;

    bool bSplitHorizontally = Size.X < Size.Y;  

    // 최소비율과 최소크기 조건 확인
    //float Ratio = FMath::FRandRange(0.3f, 0.7f); // 30% ~ 70% 사이 분할
    float Ratio = 0.5f; // 반만 분할

    if (bSplitHorizontally)
    {// 가로분할
        float SplitY = Node->MinCoordinate.Y + Size.Y * Ratio;
        Node->LeftChild = MakeShared<FBSPNode>(FBSPNode{ 
                Node->MinCoordinate, 
                FVector2D(Node->MaxCoordinate.X, SplitY), 
                (Node->MinCoordinate + FVector2D(Node->MaxCoordinate.X, SplitY)) * 0.5f
            });
        //PRINT_LOG(TEXT("My Log : %s, %s "), TEXT("LeftChild : "), *Node->LeftChild->CenterCoordinate.ToString());
        Node->RightChild = MakeShared<FBSPNode>(FBSPNode{ 
                FVector2D(Node->MinCoordinate.X, SplitY), 
                Node->MaxCoordinate, 
                (FVector2D(Node->MinCoordinate.X, SplitY) + Node->MaxCoordinate) * 0.5f
            });
        //PRINT_LOG(TEXT("My Log : %s, %s "), TEXT("RightChild : "), *Node->RightChild->CenterCoordinate.ToString());
    }
    else
    { // 세로분할
        float SplitX = Node->MinCoordinate.X + Size.X * Ratio;
        Node->LeftChild = MakeShared<FBSPNode>(FBSPNode{ 
                Node->MinCoordinate,
                FVector2D(SplitX, Node->MaxCoordinate.Y),
                (Node->MinCoordinate + FVector2D(SplitX, Node->MaxCoordinate.Y)) * 0.5f
            });
        //PRINT_LOG(TEXT("My Log : %s, %s "), TEXT("LeftChild : "), *Node->LeftChild->CenterCoordinate.ToString());
        Node->RightChild = MakeShared<FBSPNode>(FBSPNode{
                FVector2D(SplitX, Node->MinCoordinate.Y),
                Node->MaxCoordinate,
                (FVector2D(SplitX, Node->MinCoordinate.Y) + Node->MaxCoordinate) * 0.5f
            });
        //PRINT_LOG(TEXT("My Log : %s, %s "), TEXT("RightChild : "), *Node->RightChild->CenterCoordinate.ToString());
    }
    //PRINT_LOG(TEXT("My Log : %s %s, %s %s"), TEXT("min : "), *Node->MinCoordinate.ToString(), TEXT("max : "), *Node->MaxCoordinate.ToString());

    //양쪽 자식노드 분할
    bool bLeft = Split(Node->LeftChild, MaxRoomSize);
    bool bRight = Split(Node->RightChild, MaxRoomSize);

    return bLeft && bRight;
}
