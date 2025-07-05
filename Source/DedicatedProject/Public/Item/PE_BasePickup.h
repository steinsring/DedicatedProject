// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PE_Interactable.h"
#include "PE_BasePickup.generated.h"

UCLASS()
class DEDICATEDPROJECT_API APE_BasePickup : public AActor, public IPE_Interactable
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> SphereCollision; // 액터의 콜리젼

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Mesh; // 액터의 메시

	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* Interactor);
	
public:	
	// Sets default values for this actor's properties
	APE_BasePickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	FORCEINLINE USphereComponent* GetSphereCollision() const { return SphereCollision; }

	FORCEINLINE UStaticMeshComponent* GetMesh() const { return Mesh; }

	virtual void Interact(AActor* Interactor) override;
};
