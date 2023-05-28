#pragma once
#include "CoreMinimal.h"
#include "ProjectileShooterComponent.h"

class ISShooter
{
public:
    virtual ~ISShooter();
    // Initializes projectile spawners. Note: ProjectileSpawners must be set first
    virtual void InitializeSShooter(USceneComponent * OriginComponent);
    
    virtual UProjectileShooterComponent * GetProjectileSpawner(const FString& SpawnerName);
    virtual UProjectileShooterComponent * GetProjectileSpawner(const int& SpawnerIndex);
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const = 0;

    virtual void StartShooting(UProjectileShooterComponent * Spawner);
    virtual void StopShooting(UProjectileShooterComponent * Spawner);
    virtual void SetTarget(UProjectileShooterComponent * Spawner, USceneComponent* InTarget);
    virtual void SetHomingTarget(UProjectileShooterComponent * Spawner, USceneComponent* InTarget);
    virtual void SetTargetVector(UProjectileShooterComponent * Spawner, FVector InTarget);
    
    
    virtual bool IsShooting() const;
    virtual void ApplyEffectsToAbility(UMyGameplayAbility * Ability);
    virtual void ClearEffects();
    virtual FVector GetDefaultTarget() const;
    virtual USceneComponent* GetDefaultHomingTarget() const;

    USceneComponent * ProjectileOriginSceneComponent;
    TArray<UProjectileShooterComponent *> ProjectileSpawners;

    TMap<FString, UProjectileShooterComponent *> ProjectileToShooterMap;

};
