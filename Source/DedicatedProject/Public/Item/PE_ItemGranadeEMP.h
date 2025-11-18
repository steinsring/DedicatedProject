// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/PE_ItemThrowable.h"
#include "PE_ItemGranadeEMP.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API APE_ItemGranadeEMP : public APE_ItemThrowable
{
	GENERATED_BODY()
	
public:
	APE_ItemGranadeEMP();


protected:
	virtual void BeginPlay() override;


	// 모든 클라이언트에 폭발 이펙트 실행
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayExplosionEffects();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Detect")
	float ExplosionDelay = 2.0f;          // 폭발까지 딜레이

	UPROPERTY(EditDefaultsOnly, Category = "Detect")
	bool bDestroyAfterDetect = true;   // 감지 후 파괴할지

	FTimerHandle DetectTimerHandle;

	float StunTime = 1.0f;				// 스턴 지속시간
	
	// 폭발 효과 함수
	UFUNCTION()
	void Explosion();

	UPROPERTY(EditAnywhere, Category = "FX")
	class UParticleSystem* ExplosionEffect;
};
