// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define PRINT_CALLINFO() UE_LOG(LogTemp, Warning, TEXT("%s"), *CALLINFO)
#define PRINT_LOG(fmt, ...) UE_LOG(LogTemp, Warning, TEXT("%s, %s"), *CALLINFO, *FString::Printf(fmt, ##__VA_ARGS__))
#define PRINT_ERROR_LOG(fmt, ...) UE_LOG(LogTemp, Error, TEXT("%s, %s"), *CALLINFO, *FString::Printf(fmt, ##__VA_ARGS__))
//PRINT_LOG(TEXT("My Log : %s, %s "), TEXT("LeftNode : "), *LeftNodeCenterCoordinate.ToString());

