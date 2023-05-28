// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileShooterComponent.h"
#include "PShooter_RotationalBarrage.generated.h"

/**
 * 
 */


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSGAME_API UPShooter_RotationalBarrage : public UProjectileShooterComponent
{
	GENERATED_BODY()


public:
	UPShooter_RotationalBarrage();
	virtual void StartShooting() override;
	virtual void StopShooting() override;
	virtual bool PreShot() override;


	
protected:
	virtual void OnShotSpawn(AProjectileActor * SpawnedObject) override;
	virtual void DoShot() override;
	virtual void OnEndBarrage() override;


	UPROPERTY(EditAnywhere)
	float TimeBetweenShots = 0.1f;

	UPROPERTY(EditAnywhere)
	int NumShots = 5;


	UPROPERTY(EditAnywhere)
	FVector RotStart = FVector(0, -50, 0);

	UPROPERTY(EditAnywhere)
	FVector RotEnd = FVector(0, 50, 0);

	int CurShot = 0;
	float T = 0;

	FVector DifferenceVec;

	virtual FVector GetInitialBarrageDirection() const;

	virtual void UpdateBarrageDirection(float Tick);

	
	
};
