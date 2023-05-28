// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayMontageAbility.h"

#include "PlayMontageCharacterAbility.generated.h"

/**
 * 
 */
class ASHumanoidCharacter;
class ASHumanoidPlayerCharacter;
class AConfectioniteCharacter;
class AWeaponActor;



// PlayMontage for specifically Humanoid characters 
UCLASS()
class SOULSGAME_API UPlayMontageCharacterAbility : public UPlayMontageAbility
{
	GENERATED_BODY()
protected:

	virtual void PreActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual bool HandleGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData* Payload) override;


	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	


};


