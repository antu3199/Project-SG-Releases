#pragma once
#include "GameplayTagContainer.h"
#include "SGAbilityTypes.h"
#include "SoulsGame/SGTeam.h"
#include "SGStatComponent.h"
#include "SGEffect.generated.h"

USTRUCT(BlueprintType)
struct FSGEffectInstigatorContext
{
	GENERATED_BODY()

	friend class USGEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class USGEffect> EffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 1;
	
	/** The actor that owns the abilities, shouldn't be null */
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor>	OwnerActor;

	/** The physical representation of the owner, used for targeting and animation. This will often be null! */
	/** Get "body" of task's owner / default, having location in world (e.g. Owner = AIController, Avatar = Pawn) */
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor>	AvatarActor;

	// Instigating ability, optional.
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<class USGAbility> InstigatingAbility;
	
	class USGAbilityComponent* GetAbilityComponent() const;

	bool IsValid() const
	{
		return OwnerActor.IsValid();
	}

	UPROPERTY(BlueprintReadOnly, DisplayName="Effect Instance (Only value when given)")
	TWeakObjectPtr<class USGEffect> EffectInstance;
};

USTRUCT(BlueprintType)
struct FSGEffectEvaluationContext
{
	GENERATED_BODY()
	
	// Note: TArray<WeakObjectPtr> is not supported, so allow only single target for now.
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor>	TargetActor;

	UPROPERTY(BlueprintReadWrite, Category="Effect.ApplicationType")
	FGameplayTag AutomaticApplicationType;
};

USTRUCT(BlueprintType)
struct FSGStatModifierEffectParams
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="StatModifiers")
	TArray<FSGStatModifier> StatModifiers;
};

UCLASS(BlueprintType, Blueprintable)
class USGEffect : public UObject
{
	GENERATED_BODY()

	friend class USGAbilityComponent;

public:
	USGEffect(const FObjectInitializer& ObjectInitializer);
	
	static USGEffect* NewSGEffect(UObject* Outer, const FSGEffectInstigatorContext& Context);

	void InitEffect(const FSGEffectInstigatorContext& Context);

	UFUNCTION(BlueprintCallable)
	bool RequestApplyEffect();

	// Normal function to "End" an effect (or decrease stacks)
	UFUNCTION(BlueprintCallable)
	void RequestEndEffect();

	// Force ends the effect. Use sparingly
	UFUNCTION(BlueprintCallable)
	void ForceEndEffect();

	UFUNCTION(BlueprintNativeEvent)
	bool QueryCanApplyEffect();

	// State query ===
	UFUNCTION(BlueprintCallable)
	const FGameplayTag& GetIdentifierTag() const { return IdentifierTag; }

	UFUNCTION(BlueprintCallable)
	void AddStatModifier(FSGStatModifier Modifier);

	UFUNCTION(BlueprintCallable)
	void SetLevel(int32 InLevel);

	UFUNCTION(BlueprintCallable)
	int32 GetLevel() const;

	UFUNCTION(BlueprintCallable)
	int32 GetNumStacks() const { return NumStacks; }

	UFUNCTION(BlueprintCallable)
	FSGEffectInstigatorContext GetInstigatorContext() const
	{
		check(EffectContext.IsSet())
		return EffectContext.GetValue();
	}

	UFUNCTION(BlueprintCallable)
	FSGEffectEvaluationContext GetEvaluationContext() const
	{
		check(EvaluationContext.IsSet())
		return EvaluationContext.GetValue();
	}

	// Called only if we have auto ApplicationType Effect.ApplicationType.PreDamage
	// Note: The weird format is due to blueprint restrictions...
	UFUNCTION(BlueprintNativeEvent)
	FSGDamageParams OnPreSelfDamage(const FSGDamageParams& InDamageParams);

	UFUNCTION(BlueprintNativeEvent)
	void OnPostSelfDamage(const FSGDamageParams& InDamageParams);
	
	UFUNCTION(BlueprintNativeEvent)
	FSGDamageParams OnPreDealDamage(const FSGDamageParams& InDamageParams);

	UFUNCTION(BlueprintNativeEvent)
	void OnPostDealDamage(const FSGDamageParams& InDamageParams);
	
	
	bool GetIsStackable() const { return bIsStackable; }
	
	void SetModifiersFromParams(const FSGStatModifierEffectParams Params);

	ESGEffectDurationType GetEffectDurationType() const { return EffectDurationType; }

	const FGameplayTagContainer& GetAutomaticEffectApplicationTypes() const
	{
		return AutomaticEffectApplicationTypes;
	}
	
protected:
	// Gives the player the associated effect. This must be called first before any other request
	// Note: Only the AbilityComponent should call this... 
	void RequestGiveEffect(FSGEffectEvaluationContext InEvaluationContext, bool bApplyEffectOnGive = false);
	
	void EndEffect();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnGiveEffect();
	
	UFUNCTION(BlueprintNativeEvent)
	bool OnApplyEffect();


	UFUNCTION(BlueprintNativeEvent)
	void OnStackChanged(int32 OldStacks, int32 NewStacks);
	
	UFUNCTION(BlueprintNativeEvent)
	void OnEndEffect();
	
	UPROPERTY(EditAnywhere, meta = (DisplayPriority = 1, Categories = "Effect"))
	FGameplayTag IdentifierTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect.ApplicationType")
	FGameplayTagContainer AutomaticEffectApplicationTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="Stat Modifiers")
	TArray<FSGStatModifier> StatModifiers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="Effect Duration Type")
	ESGEffectDurationType EffectDurationType;

	UPROPERTY(EditAnywhere)
	bool bIsStackable = false;
	
	TMap<FGameplayTag, int32> ModifierHandles;
	
	int32 Level = 1;
	int32 NumStacks = 0;
	
	TOptional<FSGEffectInstigatorContext> EffectContext;

	// Evaluation context. Only after calling Apply()
	TOptional<FSGEffectEvaluationContext> EvaluationContext;

	bool bIsCDO = true;
	
};
