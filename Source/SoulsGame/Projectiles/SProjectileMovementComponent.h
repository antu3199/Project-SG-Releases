// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SProjectileMovementComponent.generated.h"

/**
* 
*/
UCLASS()
class SOULSGAME_API USProjectileMovementComponent : public UProjectileMovementComponent
{
    GENERATED_BODY()

    public:
    USProjectileMovementComponent(const FObjectInitializer& ObjectInitializer);

    protected:

    UPROPERTY(EditAnywhere)
    float AccelerationOverride;

    virtual FVector ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const override;
};
