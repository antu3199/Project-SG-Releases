#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTasksComponent.h"
#include "SGAbility.h"
#include "SGEffect.h"
#include "SoulsGame/SGHandleGenerator.h"


#include "SGAbilityComponent.generated.h"

class USGAbility;

USTRUCT(BlueprintType)
struct FSGAbilitySpec
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<USGAbility> AbilityClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 1;

	// Note: Need to put it here for instances
	// E.g. what if AI wants to have 2 active ability instances, but on a cooldown?
	float LastActivationTime = -1;

	// Not really necessary, but useful to know
	bool bIsInstancedOnActivation = true;

	FGameplayTag GetTag() const;

	bool IsValid() const
	{
		return AbilityClass != nullptr;
	}
};

USTRUCT(BlueprintType)
struct FSGAbilityActivationParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<AActor> AvatarActor;
};

USTRUCT(BlueprintType)
struct FSGAbilityInstanceWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Handle = INDEX_NONE;
	
	UPROPERTY()
	USGAbility* Ability = nullptr;

	FGameplayTag GetTag() const
	{
		if (Ability)
		{
			return Ability->GetIdentifierTag();
		}

		return FGameplayTag();
	}
};



UCLASS()
class SOULSGAME_API USGAbilityComponent : public UGameplayTasksComponent
{
	GENERATED_BODY()

	friend class USGAbility;
	friend class USGEffect;
public:
	USGAbilityComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	// Actions:
	void GiveAbility(TSubclassOf<USGAbility> AbilityClass, int32 Level = 1);
	int32 RequestActivateAbilityByTag(const FGameplayTag& Tag, AActor* AvatarActor = nullptr);
	int32 RequestActivateAbilityBySpec(const FSGAbilitySpec* Spec, AActor* AvatarActor = nullptr);

	// State query
	bool HasGivenAbility(const FGameplayTag& Tag) const;
	void GetGivenAbilities(TMap<FGameplayTag, FSGAbilitySpec>& OutAbilities) const;
	FSGAbilitySpec* GetGivenAbilitySpecByTag(const FGameplayTag& Tag);

	USGAbility* GetAbilityInstanceById(int32 InstanceHandle) const;
	USGAbility* GetAbilityInstanceByTag(const FGameplayTag& Tag) const;
	void GetAbilityInstancesByTag(const FGameplayTag& Tag, TArray<USGAbility*>& OutAbilities) const;
	
	bool IsAbilityActive(const int32& AbilityInstanceHandle) const;
	bool IsAbilityActive(const FGameplayTag& Tag) const;

	void GetActivatableAbilities(TArray<FSGAbilitySpec>& OutAbilities) const;
	bool CanActivateAbilityWithTag(const FGameplayTag& Tag) const;

	// Callbacks
	void HandleAbilityEvent(const struct FSGAbilityEventData* Payload);
	void UnGiveAbilityByTag(const FGameplayTag& Tag);

	virtual void SetAbilityLevel(const FGameplayTag& AbilityTag, int32 Level);

	// Cooldowns
	UFUNCTION(BlueprintCallable)
	float GetCurrentCooldownTime(const FGameplayTag& Tag) const;

	// Useful for UI
	UFUNCTION(BlueprintCallable)
	float GetCurrentCooldownPercent(const FGameplayTag& Tag) const;
	
	float GetLastActivationTime(const FGameplayTag& Tag) const;
	void RequestCooldownStart(const FGameplayTag& Tag);
	bool IsUnderCooldown(const FGameplayTag& Tag) const;

	
	static FGameplayTag GetTagFromAbilityClass(const TSubclassOf<class USGAbility>& AbilityClass);

	// EFFECTS ===
	UFUNCTION(BlueprintCallable)
	USGEffect* GiveEffect(USGEffect* EffectInstance, const FSGEffectEvaluationContext& EffectEvaluationContext);
	
	UFUNCTION(BlueprintCallable)
	bool HasActiveEffect(const FGameplayTag& Tag) const;

	UFUNCTION(BlueprintCallable)
	USGEffect* GetActiveEffect(const FGameplayTag& Tag) const;

	UFUNCTION(BlueprintCallable)
	void RemoveEffectByTag(UPARAM(meta = (Categories = "Effect"))const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable)
	void RemoveEffectByInstance(UPARAM(meta = (Categories = "Effect"))USGEffect* Effect);
	
	USGEffect* CreateEffectInstance(const TSubclassOf<class USGEffect>& EffectClass, const FSGEffectInstigatorContext& EffectContext);

	void GetEffectsWithApplicationType(const FGameplayTag& EffectApplicationType, TArray<USGEffect*>& OutEffects);

	

	
	// ===
	
protected:
	
	void OnAbilityInstanceEnded(USGAbility* AbilityInstance);
	void OnEffectInstanceEnded(USGEffect* EffectInstance);
	
	void RemoveAbilityInstance(USGAbility* AbilityInstance);
	
	int32 CreateAbilityInstance(const FSGAbilitySpec& AbilitySpec);

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<USGAbility>> DefaultGivenAbilities;
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class USGEffect>> DefaultGivenEffects;
	
	UPROPERTY(Transient)
	TMap<FGameplayTag, FSGAbilitySpec> GivenAbilities;

	UPROPERTY(Transient)
	TMap<int32, FSGAbilityInstanceWrapper> AbilityInstances;

	// All status effects are tag unique. If you need stacks, use the stacking system.
	UPROPERTY(Transient)
	TMap<FGameplayTag, class USGEffect*> ActiveStatusEffects;

	TMap<FGameplayTag, TSet<int32>> AbilityTagToInstanceHandles;
	
	FSGHandleGenerator AbilityInstanceHandleGenerator;
};
