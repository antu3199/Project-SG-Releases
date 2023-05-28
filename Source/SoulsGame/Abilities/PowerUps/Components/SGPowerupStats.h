// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SGPowerupStats.generated.h"

/**
 * 
 */

USTRUCT()
struct SOULSGAME_API FSGPowerupStats
{
	GENERATED_BODY()
	virtual ~FSGPowerupStats() = default;
	
	FSGPowerupStats();

    UPROPERTY()
    int32 MaxLevel;

    UPROPERTY()
    float BaseDamage;

    UPROPERTY()
    float Speed;

    UPROPERTY()
    float Cooldown;

    UPROPERTY()
    float HitboxDelay;

    UPROPERTY()
    int32 PoolLimit;

    UPROPERTY()
    float CritMulti;

    UPROPERTY()
    int32 Rarity;

    UPROPERTY()
    float Area;

    UPROPERTY()
    int32 Amount;

    UPROPERTY()
    int32 Pierce;

    UPROPERTY()
    float Projectile_Interval;

    UPROPERTY()
    float Knockback;
    
    UPROPERTY()
    float Chance;

    UPROPERTY()
    bool CanBeBlockedByWalls;
};

