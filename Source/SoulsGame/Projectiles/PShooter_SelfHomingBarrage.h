// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PShooter_RotationalBarrage.h"
#include "PShooter_SelfHomingBarrage.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSGAME_API UPShooter_SelfHomingBarrage : public UPShooter_RotationalBarrage
{
	GENERATED_BODY()

public:
	UPShooter_SelfHomingBarrage();
	virtual void OnEndBarrage() override;
protected:
	virtual void OnShotSpawn(AProjectileActor * SpawnedObject) override;
	virtual void DoShot() override;


	UPROPERTY(EditAnywhere)
	FVector ModifiedRot = FVector(0, 90, 0);

	UPROPERTY(EditAnywhere)
	float TransformOffsetMagnitude = 10;
	
};
