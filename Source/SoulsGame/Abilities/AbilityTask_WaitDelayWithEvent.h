// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilityTask_WaitDelayWithEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAbilityTask_WaitDelayWithEventDelegate, FGameplayTag, varNameFGameplayTag, FGameplayEventData, varNameFGameplayEventData);

/**
 * An extension of AbilityTask_WaitDelayWithEvent (need to copy entire code because members are private.)
 */
UCLASS()
class SOULSGAME_API UAbilityTask_WaitDelayWithEvent : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FWaitDelayDelegate OnFinish;

	virtual void Activate() override;

	/** Return debug string describing task */
	virtual FString GetDebugString() const override;

	/** Wait specified time. This is functionally the same as a standard Delay node. */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
    static UAbilityTask_WaitDelayWithEvent* WaitDelayWithEvent(UGameplayAbility* OwningAbility, float Time, FGameplayTagContainer& Tags);

	// Triggering gameplay event
	FAbilityTask_WaitDelayWithEventDelegate OnEventReceived;

	void ExternalCancel() override;
protected:
	FDelegateHandle EventDelegateHandle;
	
	void OnTimeFinish();

	float Time;
	float TimeStarted;

	FGameplayTagContainer EventTags;
	FTimerHandle TimerHandle;

	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);
};
