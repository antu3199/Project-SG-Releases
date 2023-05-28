#pragma once
#include "CoreMinimal.h"
#include "SGProjectilePowerupComponent.h"
#include "SGProjectilePowerupComponent_SingleShot.generated.h"

UCLASS(Blueprintable)
class USGProjectilePowerupComponent_SingleShot : public USGProjectilePowerupComponent
{
	GENERATED_BODY()
	
public:
	USGProjectilePowerupComponent_SingleShot();

	virtual void PostInitProperties() override;

protected:
	virtual void RequestPowerup() override;
	virtual void StopPowerup() override;
	virtual void OnShotSpawn(AProjectileActor * SpawnedObject) override;
	virtual void OnTargetUpdated(const FVector& Direction) override;

};
