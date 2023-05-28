// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayMontageAbility.h"
#include "SoulsGame/Character/ConfectioniteCharacter.h"

#include "AbilityConfectioniteBase.generated.h"

class UAbilityTask_WaitDelayWithEvent;
/**
 * 
 */
UCLASS()
class SOULSGAME_API UAbilityConfectioniteBase : public UPlayMontageAbility
{
	GENERATED_BODY()
public:


	virtual  void PreActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	
protected:

	virtual bool HandleGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData* Payload) override;

	virtual void OnCompleted(FGameplayTag GameplayTag, FGameplayEventData GameplayEventData) override;

	
	UFUNCTION()
	void ReattachConfectioniteAndEndAbility();
	
	AConfectioniteCharacter * ConfectioniteCharacter = nullptr;
	ASHumanoidPlayerCharacter * CharacterBase = nullptr;

	void EnableWeaponHitbox(bool Enabled);

	UPROPERTY()
	UAbilityTask_WaitDelayWithEvent * AbilityTask;

	bool TaskShouldEndAbility = false;

	
};
