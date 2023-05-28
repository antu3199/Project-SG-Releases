#include "SProjectileMovementComponent.h"

USProjectileMovementComponent::USProjectileMovementComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    
}

FVector USProjectileMovementComponent::ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const
{

    FVector VelocityDirection = InVelocity;
    VelocityDirection.Normalize();
    FVector Acceleration = VelocityDirection * AccelerationOverride;

    Acceleration.Z += GetGravityZ();

    Acceleration += PendingForceThisUpdate;

    if (bIsHomingProjectile && HomingTargetComponent.IsValid())
    {
        Acceleration += ComputeHomingAcceleration(InVelocity, DeltaTime);
    }

    return Acceleration;
}
