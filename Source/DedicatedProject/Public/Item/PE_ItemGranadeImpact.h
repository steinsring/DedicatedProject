// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/PE_ItemThrowable.h"
#include "PE_ItemGranadeImpact.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDPROJECT_API APE_ItemGranadeImpact : public APE_ItemThrowable
{
	GENERATED_BODY()
public:
	APE_ItemGranadeImpact();
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Detect")
	float ExplosionDelay = 2.0f;			// 폭발까지 딜레이

	UPROPERTY(EditDefaultsOnly, Category = "Detect")
	bool bDestroyAfterDetect = true;		// 감지 후 파괴할지

	FTimerHandle DetectTimerHandle;

	//float StunTime = 1.0f;					// 스턴 지속시간

	// 폭발 효과 함수
	UFUNCTION()
	void Explosion();

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float ExplosionRadius = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float ImpulseStrength = 2500.f;            // 물리 오브젝트용 (kg*m/s)

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float CharacterLaunchStrength = 1200.f;    // 캐릭터 수평 힘

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float CharacterUpBoost = 300.f;            // 살짝 위로 띄우기
	
};
