#pragma once
#include "CoreMinimal.h"
#include "SGProjectilePowerupComponent_SingleShot.h"
#include "SGProjectilePowerupComponent_MultiShot.generated.h"

UCLASS(Blueprintable)
class USGProjectilePowerupComponent_MultiShot : public USGProjectilePowerupComponent_SingleShot
{
	GENERATED_BODY()

public:
	USGProjectilePowerupComponent_MultiShot();

protected:
	virtual void DoPowerup() override;
	virtual void OnShotSpawn(AProjectileActor * SpawnedObject) override;

};
