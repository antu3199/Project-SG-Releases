// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MyGameplayAbility.h"

#include "CharacterAbilitySystemComponent.generated.h"

#define OUT





/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULSGAME_API UCharacterAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, OUT TArray<UMyGameplayAbility*>& ActiveAbilities);
	void GetActiveAbilitiesWithTag(const FString & TagName, OUT TArray<UMyGameplayAbility*>& ActiveAbilities);
	void GetActiveAbilitiesWithTag(const FGameplayTag & Tag, OUT TArray<UMyGameplayAbility*>& ActiveAbilities);
	bool  IsUsingAbilityWithTag(const FString & Tag);
	bool ActivateAbilityWithTag(const FString & TagName);
	bool ActivateAbilityWithTag(const FGameplayTag & Tag);

	void ApplyGameplayEffect(TSubclassOf<UMyGameplayEffect> & Effect, const UObject* NewSourceObject);
	
	const TArray<FGameplayAbilitySpecWrapper> & GetGameplaySpecWrappers() const
	{
	    return this->GameplaySpecWrappers;
    }

	FGameplayAbilitySpecWrapper& GrantAbility(const TSubclassOf<UMyGameplayAbility> & Ability);
	bool GrantAndActivateAbilityOnce(const TSubclassOf<UMyGameplayAbility> & Ability, const FGameplayEventData* GameplayEventData = nullptr);
	
	// void GrantAbility(UMyGameplayAbility * Ability);
	void GrantAbilities(const TArray<TSubclassOf<UMyGameplayAbility>> & Abilities);

	bool IsUsingAnyAbility();

	void RemoveAbilityWithTag(const FGameplayTag& Tag);


	FGameplayAbilitySpecWrapper* GetGameplayWrapperWithTag(const FGameplayTag& Tag);
	FGameplayAbilitySpecWrapper* GetGameplayWrapperWithTag(const FGameplayTagContainer& Tag);



	
	
protected:

	void BeginPlay() override;
	void BeginDestroy() override;
	
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
	TArray<FGameplayAbilitySpecWrapper> GameplaySpecWrappers;
	
};
