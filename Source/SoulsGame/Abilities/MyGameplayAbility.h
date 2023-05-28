// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbility.h"
#include "MyGameplayEffect.h"
#include "MyGameplayAbility.generated.h"

class UAbilitySystemComponent;


DECLARE_MULTICAST_DELEGATE(FOnFinishedAbility);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBroadcastAbilityEvent, class UMyGameplayAbility*, Ability, FGameplayTag, EventTag, FGameplayEventData, EventPayload);

/**
 * 
 */
UCLASS(Blueprintable)
class SOULSGAME_API UMyGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

	
public:
	virtual void PostInitProperties() override;

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UMyGameplayEffect>> AppliedGameplayEffects;

	TSharedPtr<FGameplayEffectsWrapperContainer> GameplayEffectsContainer;
	
	// Wrapper around K2, because it's protected.
	TArray<FActiveGameplayEffectHandle> ApplyGameplayEffectSpecToTarget(const FGameplayEffectSpecHandle EffectSpecHandle, FGameplayAbilityTargetDataHandle TargetData);

	// -1 For use GE default, otherwise override duration
	UPROPERTY(EditAnywhere)
	float OverrideDuration = -1;

	// Add an effect using the default gameplay ability
	// Used mostly to circumvent creating a gameplay ability
	static UMyGameplayAbility * CreateDefaultAbilityWithEffect(UMyGameplayEffect * InEffect, const FString& AbilityTag);

	void SetCurrentActorInfo(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const;


	bool IsRangedAbility() const;
	bool IsMeleeAbility() const;
	bool IsSelfAbility() const;

	float GetMinimumRangeToActivate_Legacy() const;
	float GetMoveToAcceptableRadius_Legacy() const;

	float GetMaxRangeToActivate() const;
	float GetMinRangeToActivate() const;
	
	int32 GetLevel() const;
	virtual void SetLevel(int32 Level);

	// TODO: If this ever becomes a networked game, need to refactor to use GameplaySpec's level
	int32 CurrentLevel = 0;

	void SetOverrideInstigator(AActor* Other);

	UFUNCTION(BlueprintImplementableEvent)
	void HandleGameplayEvent_BP(const FGameplayTag& EventTag, const FGameplayEventData& Payload);
	
	virtual bool CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) override;

	virtual void PreActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);
	virtual void DoActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);
	virtual void PostActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);
	
	UFUNCTION(BlueprintImplementableEvent)
	void PreActivateAbility_BP();

	UFUNCTION(BlueprintImplementableEvent)
	void DoActivateAbility_BP();

	UFUNCTION(BlueprintImplementableEvent)
	void PostActivateAbility_BP();

	UFUNCTION(BlueprintImplementableEvent)
	bool CheckCost_BP(const FGameplayAbilityActorInfo& ActorInfo) const;

	UFUNCTION(BlueprintImplementableEvent)
	void ApplyCost_BP(const FGameplayAbilityActorInfo& ActorInfo) const;

	UFUNCTION(BlueprintImplementableEvent)
	void OnGiveAbility_BP(const FGameplayAbilityActorInfo ActorInfo, const FGameplayAbilitySpec& Spec);

	// Blueprint Helpers for effects
	// May be useful to move to its own UObject later.
	UFUNCTION(BlueprintCallable)
	void ApplyEffect(int32 EffectIndex);

	UFUNCTION(BlueprintCallable)
	void AddTargetToEffect(int32 EffectIndex, AActor * TargetActor);

	UFUNCTION(BlueprintCallable)
	bool CanApplyEffectToActor(int32 EffectIndex, AActor * TargetActor);

	UFUNCTION(BlueprintCallable)
	int32 GetEffectIndexForTag(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable)
	void ResetEffectTargets();

	UFUNCTION(BlueprintCallable)
	void SetEffectLevel(int32 EffectIndex, int32 Level);

	UFUNCTION(BlueprintCallable)
	void GetAllActiveGameplayEffects(TSet<FActiveGameplayEffectHandle>& OutEffects) const;

	UFUNCTION(BlueprintCallable)
	void RemoveActiveGameplayEffectHandle(FActiveGameplayEffectHandle Handle);

	UFUNCTION(BlueprintCallable)
	void SetCustomDamageCauser(AActor* Causer);

	UFUNCTION(BlueprintCallable)
	void CallNotifyEnd();

	// Event for informing other systems (AI esp) about the state of an ability.
	UPROPERTY(BlueprintAssignable)
	FOnBroadcastAbilityEvent OnBroadcastAbilityEvent;

	UFUNCTION(BlueprintCallable)
	void BroadcastAbilityEvent(UPARAM(meta = (Categories = "Ability.AbilityBroadcasts")) FGameplayTag EventTag, FGameplayEventData EventPayload);

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	
	virtual void EndAbility(const bool WasCancelled = false);
	
	virtual void InitializeEffectContainerHelper();

	virtual  bool ShouldUseAbility();

	virtual void OnBeforeEffectApply(FGameplayEffectSpecWrapper & Data, TArray<TWeakObjectPtr<AActor>> & HitActors);
	virtual void OnAfterEffectApply(FGameplayEffectSpecWrapper & Data, TArray<TWeakObjectPtr<AActor>>& HitActors);

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;

	
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	void OnGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData* Payload);

	virtual bool HandleGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData* Payload);

	virtual void OnDynamicHitboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void OnDynamicHitboxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	virtual void OnDynamicHitboxTickOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor);

	
	// Helpers for getting the character
	class ASCharacterBase* GetCharacter() const;
	
	class ASHumanoidCharacter* GetHumanoidCharacter() const;
	class AWeaponActor* GetHumanoidWeaponActor() const;
	class ASHumanoidPlayerCharacter* GetHumanoidPlayerCharacter() const;


	
	// Member functions on activation
	FGameplayAbilitySpecHandle ActivationHandle;
	FGameplayAbilityActivationInfo ActivationActivationInfo;
    const FGameplayEventData* ActivationTriggerEventData;


	UPROPERTY(EditAnywhere)
	bool bListenToEvents = true;
	
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer EventFilters;

	// Minimum distance to activate. For melee abilities, this should be -1. For range, it should be whatever else
	// About 150 is a good melee attack
	UPROPERTY(EditAnywhere, Category= "AI", meta=(DisplayName = "MaxRangeToActivate (Range for Melee attacks)"))
	float MaxRangeToActivate = -1.0f;

	// For ranged abilities
	UPROPERTY(EditAnywhere, Category= "AI", meta=(DisplayName = "MinRangeToActivate (Min range for ranged attacks)"))
	float MinRangeToActivate = -1.0f;
	
	// Minimum distance to activate. For melee abilities, this should be -1. For range, it should be whatever else
	UPROPERTY(EditAnywhere)
	float MinimumRangeToActivate_Legacy = -1.0f;

	// Distance from move to to acceptable radius
	UPROPERTY(EditAnywhere)
	float MoveToAcceptableRadius_Legacy = -1.0f;

	UPROPERTY(EditAnywhere)
	bool bBlueprintOverrideCost = false;

	// Should only be true for abilities that take control of the character
	UPROPERTY(EditAnywhere)
	bool bResetRootMotionState = true;

	FDelegateHandle EventDelegateHandle;

	TWeakObjectPtr<AActor> OverrideInstigator = nullptr;

	TArray<int32> AllHitboxIds;
	bool bEmpoweredByAbility = false;
	

	UPROPERTY(EditAnywhere)
	bool bDoNotifyEndOnAbilityEnd = true;

	UPROPERTY(EditAnywhere)
	bool bClearQueuedAbilityOnBegin = true;
};

USTRUCT()
struct FGameplayAbilitySpecWrapper
{
	GENERATED_BODY()

	void Initialize(UAbilitySystemComponent * InComponent);
	
	FGameplayAbilitySpecHandle GiveAbility(const TSubclassOf<UMyGameplayAbility>& InAbility);
	FGameplayAbilitySpecHandle GiveAbilityAndActivateOnce(const TSubclassOf<UMyGameplayAbility>& InAbility, const FGameplayEventData* GameplayEventData = nullptr);
	
	bool HasTag(const FString &InTagName) const;
	bool HasTag(const FGameplayTag &InTag) const;
	bool HasAllTags(const FGameplayTagContainer& HasAllTags) const;
	
	FString GetAbilityName() const;
	bool ActivateAbility() const;
	
	FGameplayAbilitySpecHandle GameplayAbilitySpecHandle;
	FGameplayAbilitySpec * GetAbilitySpec() const;
	// Gets the primary instanced ability
	// Probably should set instancing policy to "Instanced per actor" If you plan on doing this
	UGameplayAbility* GetPrimaryInstancedAbility(int Index = 0) const;
	UGameplayAbility* GetMostRecentInstancedAbility() const;

	bool bHasGivenAbility = false;
	bool bRemoveAbilityOnFinish = false;

	TWeakObjectPtr<UAbilitySystemComponent> CachedAbilitySystem = nullptr;
	TWeakObjectPtr<UMyGameplayAbility> Ability = nullptr;

};