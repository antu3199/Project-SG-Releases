// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayMontageCharacterAbility.h"

#include "SoulsGame/SHitboxManager.h"
#include "SoulsGame/Character/SHumanoidPlayerCharacter.h"


#include "SoulsGame/Animation/SendGameplayTagEventNS.h"


void UPlayMontageCharacterAbility::PreActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::PreActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool UPlayMontageCharacterAbility::HandleGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (Super::HandleGameplayEvent(EventTag, Payload))
	{
		return true;
	}

	return false;
}

bool UPlayMontageCharacterAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	const bool PreviousCost = Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
	if (!PreviousCost)
	{
		return false;
	}

	return true;
}

void UPlayMontageCharacterAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}

void UPlayMontageCharacterAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}



