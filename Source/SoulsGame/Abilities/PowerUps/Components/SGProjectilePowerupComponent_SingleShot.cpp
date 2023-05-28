#include "SGProjectilePowerupComponent_SingleShot.h"


USGProjectilePowerupComponent_SingleShot::USGProjectilePowerupComponent_SingleShot() : Super()
{
	Stats.BaseDamage = 10;
	Stats.Amount = 1;
}

void USGProjectilePowerupComponent_SingleShot::PostInitProperties()
{
	Super::PostInitProperties();


}


void USGProjectilePowerupComponent_SingleShot::RequestPowerup()
{
	Super::RequestPowerup();

	ShootDirection = GetShooterToTargetDirection();
	
	//DoShot();
	/*
	if (PreShot())
	{
		SingleShot(ShootDirection);
	}
	*/
}


void USGProjectilePowerupComponent_SingleShot::StopPowerup()
{
	Super::StopPowerup();
}

void USGProjectilePowerupComponent_SingleShot::OnShotSpawn(AProjectileActor* SpawnedObject)
{
	Super::OnShotSpawn(SpawnedObject);

	if (!bPlayLooping)
	{
		// If not looping, then stop immedately
		StopPowerup();
	}
	else
	{
		// If looping, only stop if this stop is last one
		if (bNextShotAsLast)
		{
			StopPowerup();
		}
	}
}

void USGProjectilePowerupComponent_SingleShot::OnTargetUpdated(const FVector& Direction)
{
	Super::OnTargetUpdated(Direction);

	ShootDirection = GetShooterToTargetDirection();
}
