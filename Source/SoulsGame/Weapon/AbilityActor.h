// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/SphereComponent.h"
#include "SoulsGame/Abilities/GameplayEffectActor.h"
#include "SoulsGame/Abilities/MyGameplayAbility.h"

#include "AbilityActor.generated.h"

UCLASS()
class SOULSGAME_API AAbilityActor : public AGameplayEffectActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAbilityActor();

	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UFUNCTION()
    virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void EnableCollision(const bool Set);

	void ToggleCollision();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* Base;

	UPROPERTY(EditAnywhere)
	bool CollisionEnabledAtStart = true;

	ECollisionEnabled::Type CollisionType;

	bool IsCollisionEnabled = false;

	UPROPERTY()
	AActor * OtherOverlappingActor = nullptr;
	bool IsOverlapping = false;


	int32 HitboxId = -1;
};
