// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SGAbilityTypes.h"
#include "SGEffect.h"

#include "SGAbilityBlueprintLibrary.generated.h"


class USGAbilityComponent;

/** Blueprint library for ability system. Many of these functions are useful to call from native as well */
UCLASS(meta=(ScriptName="AbilitySystemLibrary"))
class SOULSGAME_API USGAbilitySystemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Tries to find an ability system component on the actor, will use AbilitySystemInterface or fall back to a component search */
	UFUNCTION(BlueprintPure, Category = Ability)
	static USGAbilityComponent* GetAbilityComponent(AActor *Actor);

	UFUNCTION(BlueprintCallable, Category = Ability, Meta = (Tooltip = "This function can be used to trigger an ability on the actor in question with useful payload data."))
	static void SendAbilityEventToActor(AActor* Actor, FGameplayTag EventTag, FSGAbilityEventData Payload);

	UFUNCTION(BlueprintCallable, Category = Ability, Meta = (Tooltip = "Create effect"))
	static USGEffect* StaticCreateEffect(FSGEffectInstigatorContext InstigatorContext);
	
	UFUNCTION(BlueprintCallable, Category = Ability, Meta = (Tooltip = "Execute effect"))
	static USGEffect* StaticGiveEffect(class USGEffect* EffectInstance, FSGEffectEvaluationContext EvaluationContext);

	UFUNCTION(BlueprintCallable, Category = Ability, Meta = (Tooltip = "Create effect"))
	static USGEffect* StaticCreateModifierEffect(FSGEffectInstigatorContext InstigatorContext, FSGStatModifierEffectParams StatModifierEffectParams);

	UFUNCTION(BlueprintCallable, Category = Ability, Meta = (Tooltip = "Applies damage"))
	static void StaticApplyDamage(FSGDamageParams DamageParams);
	
	static FGameplayTag GetOnHitEffectApplicationType()
	{
		return UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Effect.ApplicationType.OnHit"), false);
	}
	
	static FGameplayTag GetOnOwnerHitEffectApplicationType()
	{
		return UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Effect.ApplicationType.OnOwnerHit"), false);
	}

	static FGameplayTag GetAttackMitigatorApplicationType()
	{
		return UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Effect.ApplicationType.AttackMitigator"), false);
	}
	
	static FGameplayTag GetDefenceMitigatorApplicationType()
	{
		return UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Effect.ApplicationType.DefenceMitigator"), false);
	}

private:
	static FAnimMontageInstance* GetMontageInstanceFromActor(const AActor* Actor);
};
