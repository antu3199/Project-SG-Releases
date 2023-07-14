// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SGProjectileMovementComponent.generated.h"

/**
* 
*/
UCLASS()
class SOULSGAME_API USGProjectileMovementComponent : public UProjectileMovementComponent
{
    GENERATED_BODY()

    public:
    USGProjectileMovementComponent(const FObjectInitializer& ObjectInitializer);

    protected:

    UPROPERTY(EditAnywhere)
    float AccelerationOverride;

    virtual FVector ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const override;
};
