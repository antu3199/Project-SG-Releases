#include "SGProjectileMovementComponent.h"

USGProjectileMovementComponent::USGProjectileMovementComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    
}

FVector USGProjectileMovementComponent::ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const
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
