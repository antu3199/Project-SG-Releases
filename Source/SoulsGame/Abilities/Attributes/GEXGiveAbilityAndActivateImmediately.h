// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEXGiveAbilityAndActivateImmediately.generated.h"

/**
 * Executions are for more complex modifiers, modifiers are simpler
 */
UCLASS()
class SOULSGAME_API UGEXGiveAbilityAndActivateImmediately : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UGEXGiveAbilityAndActivateImmediately();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
