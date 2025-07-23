// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/PE_BasePickup.h"
#include "PE_ItemPickup.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API APE_ItemPickup : public APE_BasePickup
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> SphereCollision; // 액터의 콜리젼

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Mesh; // 액터의 메시
	
public:
	APE_ItemPickup();
	FORCEINLINE USphereComponent* GetSphereCollision() const { return SphereCollision; }

	FORCEINLINE UStaticMeshComponent* GetMesh() const { return Mesh; }
	
};
