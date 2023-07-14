#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SGAbilityTypes.h"

#include "SGStatComponent.generated.h"

#define SG_MAX_LEVEL 100

UENUM(BlueprintType)
enum class ESGStatModifierType : uint8
{
	Additive,
	Multiplicative,
	// Note: Set only works on instantenous types
	Set
};

USTRUCT(BlueprintType)
struct FSGStatModifier
{
	GENERATED_BODY()

	FSGStatModifier() : ModiferType(ESGStatModifierType::Additive), BaseValueModifier(0), MinValueModifier(0), MaxValueModifier(0)
	{
	}

	FSGStatModifier(const FGameplayTag& Stat, float Value) : StatTag(Stat), BaseValueModifier(Value), MinValueModifier(0), MaxValueModifier(TNumericLimits<float>::Max())
	{
	}
	
	FSGStatModifier(const FGameplayTag& Stat, float Value, float Min, float Max) : StatTag(Stat), BaseValueModifier(Value), MinValueModifier(Min), MaxValueModifier(Max)
	{
	}
	
	float GetBaseValueModifier() const;
	float GetMinValueModifier() const;
	float GetMaxValueModifier() const;

	void SetLevel(int32 InLevel)
	{
		Level = InLevel;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Stat"))
	FGameplayTag StatTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESGStatModifierType ModiferType = ESGStatModifierType::Additive;

	// Properties for the "On Set" modifier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "EffectDurationType == ESGEffectDurationType::Instantaneous"))
	bool AffectBaseValue = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "EffectDurationType == ESGEffectDurationType::Instantaneous"))
	bool AffectMinValue = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "EffectDurationType == ESGEffectDurationType::Instantaneous"))
	bool AffectMaxValue = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESGEffectDurationType EffectDurationType = ESGEffectDurationType::Instantaneous;

	void AddValues(float& BaseValue, float& MinValue, float& MaxValue) const
	{
		BaseValue += GetBaseValueModifier();
		MinValue += GetMinValueModifier();
		MaxValue += GetMaxValueModifier();
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "BaseValueModifierCurve == nullptr"))
	float BaseValueModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* BaseValueModifierCurve = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "MinValueModifierCurve == nullptr"))
	float MinValueModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* MinValueModifierCurve = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "MaxValueModifierCurve == nullptr"))
	float MaxValueModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* MaxValueModifierCurve = nullptr;
};

USTRUCT(BlueprintType)
struct FSGStatValue
{
	GENERATED_BODY()

	FSGStatValue() : BaseValue(0), CurrentValue()
	{
	}

	FSGStatValue(float Value) : BaseValue(Value), CurrentValue(Value)
	{
	}
	
	FSGStatValue(float BaseValue, float CurrentValue) : BaseValue(BaseValue), CurrentValue(CurrentValue)
	{
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CurrentValue;
};

// Current stat state. Meant for passing around to delegates only.
USTRUCT(BlueprintType)
struct FSGStatValues
{
	GENERATED_BODY()

	FSGStatValues()
	{
	}

	FSGStatValues(float Value, float Min, float Max) : Value(Value), MinValue(Min), MaxValue(Max)
	{
	}

	bool Equals(const FSGStatValues& Other) const
	{
		return Value.CurrentValue == Other.Value.CurrentValue
			&& MinValue.CurrentValue == Other.MinValue.CurrentValue
			&& MaxValue.CurrentValue == Other.MaxValue.CurrentValue;
	}

	float GetValue() const
	{
		return Value.CurrentValue;
	}

	float GetMinValue() const
	{
		return MinValue.CurrentValue;
	}

	float GetMaxValue() const
	{
		return MaxValue.CurrentValue;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSGStatValue Value;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSGStatValue MinValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSGStatValue MaxValue;
};

USTRUCT(BlueprintType)
struct FSGStat
{
	GENERATED_BODY()
	
	FSGStat()
		: Values(0, 0, 0)
	{
		ComputeCurrentValue();
	}

	FSGStat(const FGameplayTag& Tag, float Value, float MinValue, float MaxValue)
	: Tag(Tag), Values(Value, MinValue, MaxValue)
	{
		ComputeCurrentValue();
	}

	void SetBaseValue(float Value);

	void SetBaseMaxValue(float Value);

	void SetBaseMinValue(float Value);
	
	float GetCurrentValue() const
	{
		return Values.Value.CurrentValue;
	}

	float GetCurrentMaxValue() const
	{
		return Values.MaxValue.CurrentValue;
	}

	float GetCurrentMinValue() const
	{
		return Values.MinValue.CurrentValue;
	}

	void ComputeCurrentValue();

	int32 AddModifier(const FSGStatModifier& Modifier);
	void RemoveModifier(int32 Handle);

	static FSGStat CreateInvalidStat()
	{
		FSGStat Stat;
		Stat.m_IsInvalidStat = true;
		return Stat;
	}

	bool IsInvalid() const
	{
		return m_IsInvalidStat;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Stat"))
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSGStatValues Values;

	// Used for error checking
	UPROPERTY(BlueprintReadOnly)
	bool m_IsInvalidStat = false;

private:
	
	TMap<int32, FSGStatModifier> m_StatModifiers;
	
	// Out of laziness, just keep incrementing - I won't have enough to run out...
	int32 m_CurrentModifierKey = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStatChanged, FGameplayTag, StatTag, FSGStatValues, OldStat, FSGStatValues, NewStat);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelChanged, int32, NewLevel);


UCLASS()
class USGStatComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USGStatComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual  void PostInitProperties() override;

	// Gets a raw stat. Use sparingly
	UFUNCTION(BlueprintCallable)
	FSGStat GetStatByTag_BP(UPARAM(meta = (Categories = "Stat")) FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable)
	float GetStatValue(UPARAM(meta = (Categories = "Stat")) FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable)
	float GetStatMinValue(UPARAM(meta = (Categories = "Stat")) FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable)
	float GetStatMaxValue(UPARAM(meta = (Categories = "Stat")) FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable)
	int32 AddStatModifier(UPARAM(meta = (Categories = "Stat")) FGameplayTag Tag, FSGStatModifier Modifier, bool bBroadcastStatChanged = true);

	UFUNCTION(BlueprintCallable)
	void RemoveStatModifier(UPARAM(meta = (Categories = "Stat")) FGameplayTag Tag, int32 ModifierIndex, bool bBroadcastStatChanged = true);

	UFUNCTION(BlueprintCallable)
	void GetLevelRequirements(bool bRecomputeLevels, int32& OutLevelMin, int32& OutLevelMax, float& OutLevelPercent);
	
	UPROPERTY(BlueprintAssignable)
	FOnStatChanged OnStatChanged;

	UPROPERTY(BlueprintAssignable)
	FOnLevelChanged OnLevelChanged;
	
	const FSGStat* GetStatByTag(const FGameplayTag& Tag) const;

	void ApplyDamageToSelf(const FSGDamageParams& DamageParams);

	FSGStat* GetMutableStatByTag(const FGameplayTag& Tag);
	
	FSGStat* GetHealthStat() const;
	FSGStat* GetDefenceStat() const;
	FSGStat* GetAttackStat() const;
	FSGStat* GetXPStat() const;
	
	static FGameplayTag GetHealthTag();
	static FGameplayTag GetAttackTag();
	static FGameplayTag GetDefenceTag();
	static FGameplayTag GetXPTag();

	// XP stuff:
	UFUNCTION(BlueprintCallable)
	void RecomputeLevel();

	UFUNCTION(BlueprintCallable)
	int32 GetLevel() const
	{
		return LevelCached;
	}

	
protected:

	void InitializeStats();

	void HandleStatChanged(FSGStat& Stat,
		const FSGStatValues& OldStat,
		const FSGStatValues& NewStat);


	void InitializeXPCurve();

	void ApplyBaseStatEffects(int32 Level);

	// Cached values of stats for convinence
	FSGStat* HealthStat = nullptr;
	FSGStat* DefenceStat = nullptr;
	FSGStat* AttackStat = nullptr;
	FSGStat* XPStat = nullptr;
	
	TMap<FGameplayTag, FSGStat> Stats;

	UPROPERTY(EditAnywhere)
	UCurveFloat* XPCurve;

	// Effects that are reapplied on level up
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class USGEffect>> BaseStatEffects;
	
	
	int32 LevelCached = 1;

	TArray<int32> CachedXPSumArray;
};
