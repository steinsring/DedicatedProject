// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PE_ItemThrowable.generated.h"

UCLASS()
class DEDICATEDPROJECT_API APE_ItemThrowable : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "ItemThrowable")
	TObjectPtr<UStaticMeshComponent> ItemMesh;							// 아이템 메시

	//UPROPERTY(VisibleAnywhere, Category = "ItemThrowable")
	//TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;	// 투사체 로직

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "ItemThrowable")
	TObjectPtr<class USphereComponent> ItemCollision;					// 감지 영역

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	APE_ItemThrowable();

	FORCEINLINE USphereComponent* GetItemThrowableCollision() const { return ItemCollision; }
	FORCEINLINE UStaticMeshComponent* GetItemThrowableMesh() const { return ItemMesh; }
	//FORCEINLINE UProjectileMovementComponent* GetItemProjectile() const { return ProjectileMovement; }
	
};
