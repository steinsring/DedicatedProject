// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PE_ItemThrowableComponent.generated.h"

class UInputMappingContext;
class UInputAction;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEDICATEDPROJECT_API UPE_ItemThrowableComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TSubclassOf<class APE_ItemThrowable> ItemThrowable;							// 던질 아이템 클래스

public:	
	// Sets default values for this component's properties
	UPE_ItemThrowableComponent();

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetProjectileClass(TSubclassOf<class APE_ItemThrowable> NewItemThrowable);	// 스폰되는 클래스

	void Throw();																	// 던질때 클라에서 실행

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Throw_Server();															// 던질때 서버에서 실행

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
