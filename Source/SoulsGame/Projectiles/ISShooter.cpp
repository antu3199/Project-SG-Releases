#include "ISShooter.h"

ISShooter::~ISShooter()
{
}

void ISShooter::InitializeSShooter(USceneComponent * OriginComponent)
{
    ProjectileOriginSceneComponent = OriginComponent;
    int Len = ProjectileSpawners.Num();

    for (int i = 0; i < Len; i++)
    {
        UProjectileShooterComponent* Spawner = ProjectileSpawners[i];
        Spawner->Initialize(OriginComponent);
        ProjectileToShooterMap.Add(Spawner->GetName(), Spawner);
    }
}


FVector ISShooter::GetDefaultTarget() const
{
    return FVector::ForwardVector;
}

USceneComponent* ISShooter::GetDefaultHomingTarget() const
{
    return nullptr;
}

UProjectileShooterComponent* ISShooter::GetProjectileSpawner(const FString& ProjectileName)
{
    if (ProjectileToShooterMap.Contains(ProjectileName))
    {
        return ProjectileToShooterMap[ProjectileName];
    }

    return nullptr;
}

UProjectileShooterComponent* ISShooter::GetProjectileSpawner(const int& ShotIndex)
{
    if (ShotIndex >= 0 && ShotIndex < ProjectileSpawners.Num())
    {
        return ProjectileSpawners[ShotIndex];
    }

    return nullptr;
}

UAbilitySystemComponent* ISShooter::GetAbilitySystemComponent() const
{
    return nullptr; // Implement me!
}


void ISShooter::StartShooting(UProjectileShooterComponent* Spawner)
{
    if (!Spawner)
    {
        return;
    }

    Spawner->StartShooting();
}

void ISShooter::StopShooting(UProjectileShooterComponent* Spawner)
{
    if (!Spawner)
    {
        return;
    }

    Spawner->StopShooting();
}

void ISShooter::SetTarget(UProjectileShooterComponent* Spawner, USceneComponent* InTarget)
{
    if (!Spawner)
    {
        return;
    }

    Spawner->SetTarget(InTarget);
}

void ISShooter::SetHomingTarget(UProjectileShooterComponent* Spawner, USceneComponent* InTarget)
{
    if (!Spawner)
    {
        return;
    }

    Spawner->SetHomingTarget(InTarget);
}

void ISShooter::SetTargetVector(UProjectileShooterComponent* Spawner, FVector InTarget)
{
    if (!Spawner)
    {
        return;
    }

    Spawner->SetTargetVector(InTarget);
}


bool ISShooter::IsShooting() const
{
    for (auto & Spawner : ProjectileSpawners)
    {
        if (Spawner && Spawner->IsShooting) return true;
    }

    return false;
}

void ISShooter::ApplyEffectsToAbility(UMyGameplayAbility* Ability)
{
    for (auto & Spawner : ProjectileSpawners)
    {
        Spawner->ActivateDebugEffects(Ability);
    }
}

void ISShooter::ClearEffects()
{
    for (auto & Spawner : ProjectileSpawners)
    {
        Spawner->ClearEffects();
    }
}

