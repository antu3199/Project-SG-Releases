// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGPrimaryDataAsset.h"
#include "SGWeaponAsset.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGWeaponAsset : public USGPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Weapon actor to spawn
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	TSubclassOf<AActor> WeaponActorTemplate;
};
