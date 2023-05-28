// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileShooterComponent.h"
#include "PShooter_SingleShot.generated.h"

/**
 * 
 */


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSGAME_API UPShooter_SingleShot : public UProjectileShooterComponent
{
	GENERATED_BODY()


public:
	UPShooter_SingleShot();

	virtual void StartShooting() override;
	virtual void StopShooting() override;
	virtual void DoShot() override;
	virtual void OnShotSpawn(AProjectileActor * SpawnedObject) override;
	virtual void OnTargetUpdated(const FVector& Direction) override;

protected:

	
	
};
