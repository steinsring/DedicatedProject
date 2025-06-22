// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PE_BasePickup.generated.h"

UCLASS()
class DEDICATEDPROJECT_API APE_BasePickup : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> SphereCollision; // 액터의 콜리젼

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Mesh; // 액터의 메시
	
public:	
	// Sets default values for this actor's properties
	APE_BasePickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult); //캐릭터와 액터가 겹칠때 실행

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Picup", meta = (DisplayName = "Pickup"))
	void Pickup(class AProjectPlayer* OwningCharacter);  //픽업 액션

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE USphereComponent* GetSphereCollision() const { return SphereCollision; }

	FORCEINLINE UStaticMeshComponent* GetMesh() const { return Mesh; }
	
	
};
