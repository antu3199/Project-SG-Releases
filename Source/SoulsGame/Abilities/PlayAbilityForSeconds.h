// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameplayAbility.h"
#include "PlayAbilityForSeconds.generated.h"

class UAbilityTask_WaitDelayWithEvent;
/**
 * 
 */
UCLASS()
class SOULSGAME_API UPlayAbilityForSeconds : public UMyGameplayAbility
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere)
	FGameplayTagContainer EventTags;
	
	virtual void PreActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	
	UPROPERTY(EditAnywhere)
	float Time = 1.0f;
	
	UPROPERTY()
	UAbilityTask_WaitDelayWithEvent * AbilityTask;

	UFUNCTION()
	virtual void OnFinished();

	UFUNCTION()
    virtual void OnEventReceived(FGameplayTag GameplayTag, FGameplayEventData GameplayEventData);
	
};
