// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameplayEffect.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "SoulsGame/SGTeam.h"


#include "MyGameplayEffect.generated.h"

class UMyGameplayEffect;
class ASCharacterBase; 
class UMyGameplayAbility;

struct FGameplayEffectsWrapperContainer;

DECLARE_DELEGATE(OnGameplayEffectAppliedDelegate)

// Helper to organize gameplay abilities
USTRUCT()
struct FGameplayEffectSpecWrapper
{
	GENERATED_BODY()
	
	FGameplayEffectSpecWrapper();
	~FGameplayEffectSpecWrapper();
	FGameplayEffectSpecWrapper(UMyGameplayAbility * InAbility, UMyGameplayEffect * InEffect, FGameplayEffectsWrapperContainer* InParent = nullptr);

	void Initialize(UMyGameplayAbility * InAbility, UMyGameplayEffect * InEffect, FGameplayEffectsWrapperContainer* InParent = nullptr);

	void ApplyEffectToTargetData();
	void AddTarget(AActor * TargetActor, bool bCheckEffectOverlapBehaviour = true);
	void ClearTargets();

	bool ValidOverlapTarget(AActor* TargetActor) const;

	UPROPERTY(Transient)
	UMyGameplayEffect * Ability = nullptr; // Useful only if this data container is only one
	// Always check for null before using this
	UPROPERTY(Transient)
	UMyGameplayEffect * Effect = nullptr; // Useful only if this data container is only one
	
	FGameplayEffectSpecHandle GameplayEffectSpecHandle = FGameplayEffectSpecHandle();
	TSet<FActiveGameplayEffectHandle> ActiveGameplayEffectHandles = TSet<FActiveGameplayEffectHandle>();
	FGameplayAbilityTargetDataHandle TargetData = FGameplayAbilityTargetDataHandle();
	FGameplayEffectsWrapperContainer* ParentContainer = nullptr;
	TArray<TWeakObjectPtr<AActor>> CachedTargets;
};

DECLARE_DELEGATE_TwoParams(FOnBeforeApplyEffectDelegate, FGameplayEffectSpecWrapper& Data, TArray<TWeakObjectPtr<AActor>>& HitActors);
DECLARE_DELEGATE_TwoParams(FOnAfterApplyEffectDelegate, FGameplayEffectSpecWrapper& Data, TArray<TWeakObjectPtr<AActor>>& HitActors);


USTRUCT()
struct FGameplayEffectsWrapperContainer
{
	GENERATED_BODY()

	void Initialize(AActor * InInstigatorActor);
	
	FGameplayEffectSpecWrapper & AddEffect(UMyGameplayAbility * InAbility, UMyGameplayEffect * InEffect, float Duration = -1, int Level = 0);
	bool HasEffects() const;
	void Reset();

	void ApplyEffect(int32 EffectIndex);
	void AddTargetToEffect(int32 EffectIndex, AActor * TargetActor);
	bool CanApplyEffectToActor(int32 EffectIndex, AActor * TargetActor);

	int32 GetEffectIndexForTag(FGameplayTag Tag) const;

	void ResetTargets();

	void SetEffectLevel(int32 EffectIndex, int32 Level);

	void GetAllActiveGameplayEffects(TSet<FActiveGameplayEffectHandle>& OutEffects) const;
	void RemoveActiveGameplayEffectHandle(FActiveGameplayEffectHandle Handle);

	void SetCustomDamageCauser(AActor* Causer);
	
	TArray<FGameplayEffectSpecWrapper> Effects;

	FOnBeforeApplyEffectDelegate OnBeforeApplyEffectDelegate = nullptr;
	FOnAfterApplyEffectDelegate OnAfterApplyEffectDelegate = nullptr;

	TWeakObjectPtr<AActor> InstigatorActor = nullptr;
};


/**
 * 
 */
UCLASS()
class SOULSGAME_API UMyGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	ESGTeamOverlapBehaviour OverlapBehaviour = ESGTeamOverlapBehaviour::OverlapAll;
	
	static void GetTargets_UseEventData(ASCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors);
	
};
