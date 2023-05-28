// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameplayAbility.h"
#include "SGApplyEffectsToSelf.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGApplyEffectsToSelf : public UMyGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void DoActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	virtual void PreActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void EndAbility(const bool WasCancelled = false) override;
	
	void OnGameplayEffectRemoved(const FActiveGameplayEffect& ActiveGameplayEffect);

	FDelegateHandle EffectRemovedHandle;
	
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* NiagaraEffectTemplate;

	UPROPERTY(Transient)
	class UNiagaraComponent* SpawnedNiagaraComponent = nullptr;
	
};
