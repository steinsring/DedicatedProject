// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PE_Interactable.h"
#include "PE_LevelTravelManager.generated.h"

UCLASS()
class DEDICATEDPROJECT_API APE_LevelTravelManager : public AActor, public IPE_Interactable
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class USphereComponent> InteractRange;			// 액터의 콜리젼

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> InteractMesh; // 액터의 메시

public:	
	// Sets default values for this actor's properties
	APE_LevelTravelManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact(class AActor* Interactor) override;
	
};
