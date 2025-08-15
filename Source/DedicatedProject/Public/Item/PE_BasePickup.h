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
	UFUNCTION(Server, Reliable)
	void ServerInteract(class AActor* Interactor);

protected:
	UPROPERTY(EditAnywhere)
	FName PickupID = "Test";

public:
	virtual void Interact(class AActor* Interactor) override;

};
