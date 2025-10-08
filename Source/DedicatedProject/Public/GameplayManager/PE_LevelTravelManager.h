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

	// 레벨 관리----------------------------------------------------------------------------
	UFUNCTION(Server,Reliable)
	void LevelTravel();

	UFUNCTION(NetMulticast, Reliable)					// 모든 클라+서버에서 실행
	void Multicast_NotifyLevelTravelTriggered(int32 NextStage);

	bool bTravelInProgress = false;						// 재진입 가드

protected:
	virtual void BeginPlay() override;

public:	
	APE_LevelTravelManager();
	virtual void Tick(float DeltaTime) override;
	virtual void Interact(class AActor* Interactor) override;
	
};
