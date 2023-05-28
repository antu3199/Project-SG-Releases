// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileShooterComponent.h"
#include "PShooter_SingleShot.h"
#include "PShooter_MultiShot.generated.h"

/**
 * 
 */


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSGAME_API UPShooter_MultiShot : public UPShooter_SingleShot
{
	GENERATED_BODY()


public:
	UPShooter_MultiShot();
	virtual void StartShooting() override;
	virtual void StopShooting() override;
	virtual void DoShot() override;
	virtual void OnShotSpawn(AProjectileActor * SpawnedObject) override;
	virtual void OnTargetUpdated(const FVector& Direction) override;

	
};
