// Fill out your copyright notice in the Description page of Project Settings.


#include "PShooter_SingleShot.h"

UPShooter_SingleShot::UPShooter_SingleShot() : Super()
{
}

void UPShooter_SingleShot::StartShooting()
{
	Super::StartShooting();

	ShootDirection = GetShooterToTargetDirection();

	DoShot();
	/*
	if (PreShot())
	{
		SingleShot(ShootDirection);
	}
	*/
}

void UPShooter_SingleShot::StopShooting()
{
	Super::StopShooting();
	// I think base is fine
}

void UPShooter_SingleShot::DoShot()
{
	Super::DoShot();
	// Base is fine
}

void UPShooter_SingleShot::OnShotSpawn(AProjectileActor* SpawnedObject)
{
	Super::OnShotSpawn(SpawnedObject);

	if (!bPlayLooping)
	{
		// If not looping, then stop immedately
		StopShooting();
	}
	else
	{
		// If looping, only stop if this stop is last one
		if (bNextShotAsLast)
		{
			StopShooting();
		}
	}
}

void UPShooter_SingleShot::OnTargetUpdated(const FVector& Direction)
{
	Super::OnTargetUpdated(Direction);

	ShootDirection = GetShooterToTargetDirection();
}

