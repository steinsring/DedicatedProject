// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PE_WeaponProjectileComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEDICATEDPROJECT_API UPE_WeaponProjectileComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TSubclassOf<class APE_BaseWeaponProjectile> ProjectileClass;							// 던질 아이템 클래스

public:	
	// Sets default values for this component's properties
	UPE_WeaponProjectileComponent();

	UFUNCTION(Category = "Projectile")	// 스폰되는 클래스
	void SetProjectileClass(TSubclassOf<APE_BaseWeaponProjectile> NewProjectileClass)
	{	ProjectileClass = NewProjectileClass;	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Fire();																		// 클라에서 실행
private:
	UFUNCTION(Server, Reliable)
	void Fire_Server();																	// 클라에서 실행
};
