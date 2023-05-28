// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayAbilityForSeconds.h"

#include "AbilityTask_WaitDelayWithEvent.h"



void UPlayAbilityForSeconds::PreActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::PreActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AbilityTask = UAbilityTask_WaitDelayWithEvent::WaitDelayWithEvent(this, Time, EventTags);
	AbilityTask->OnEventReceived.AddDynamic(this, &UPlayAbilityForSeconds::OnEventReceived);
	AbilityTask->OnFinish.AddDynamic(this, &UPlayAbilityForSeconds::OnFinished);
	AbilityTask->ReadyForActivation();
}

void UPlayAbilityForSeconds::OnFinished()
{
}

void UPlayAbilityForSeconds::OnEventReceived(FGameplayTag GameplayTag, FGameplayEventData GameplayEventData)
{
}


