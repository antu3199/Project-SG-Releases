#include "SGStatComponent.h"

#include "GameplayTagsManager.h"
#include "SGAbilityComponent.h"
#include "SoulsGame/Character/SGCharacterBase.h"


USGStatComponent::USGStatComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InitializeStats();
}

void USGStatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (XPCurve != nullptr)
	{
		RecomputeLevel();
		ApplyBaseStatEffects(LevelCached);
	}
	else
	{
		// Default stats
		ApplyBaseStatEffects(1);
	}
}

void USGStatComponent::PostInitProperties()
{
	Super::PostInitProperties();

	if (XPCurve != nullptr && CachedXPSumArray.IsEmpty())
	{
		InitializeXPCurve();
	}
}

FSGStat USGStatComponent::GetStatByTag_BP(FGameplayTag Tag) const
{
	if (const FSGStat* Stat = GetStatByTag(Tag))
	{
		return *Stat;
	}
	
	return FSGStat::CreateInvalidStat();
}

float USGStatComponent::GetStatValue(FGameplayTag Tag) const
{
	if (const FSGStat* Stat = GetStatByTag(Tag))
	{
		return Stat->GetCurrentValue();
	}

	checkf(false, TEXT("Unable to get stat with tag %s"), *Tag.ToString());
	return 0.0f;
}

float USGStatComponent::GetStatMinValue(FGameplayTag Tag) const
{
	if (const FSGStat* Stat = GetStatByTag(Tag))
	{
		return Stat->GetCurrentMinValue();
	}

	checkf(false, TEXT("Unable to get stat with tag %s"), *Tag.ToString());
	return 0.0f;
}

float USGStatComponent::GetStatMaxValue(FGameplayTag Tag) const
{
	if (const FSGStat* Stat = GetStatByTag(Tag))
	{
		return Stat->GetCurrentMaxValue();
	}

	checkf(false, TEXT("Unable to get stat with tag %s"), *Tag.ToString());
	return 0.0f;
}

int32 USGStatComponent::AddStatModifier(FGameplayTag Tag, FSGStatModifier Modifier, bool bBroadcastStatChanged)
{
	if (FSGStat* Stat = GetMutableStatByTag(Tag))
	{
		if (!bBroadcastStatChanged)
		{
			return Stat->AddModifier(Modifier);
		}
		else
		{
			const FSGStatValues OldValues = Stat->Values;
			const int32 Handle = Stat->AddModifier(Modifier);
			const FSGStatValues NewValues = Stat->Values;

			if (!OldValues.Equals(NewValues))
			{
				HandleStatChanged(*Stat, OldValues, NewValues);
			}
		
			return Handle;
		}

	}

	checkf(false, TEXT("Unable to get stat with tag %s"), *Tag.ToString());
	return INDEX_NONE;
}

void USGStatComponent::RemoveStatModifier(FGameplayTag Tag, int32 ModifierIndex, bool bBroadcastStatChanged)
{
	if (FSGStat* Stat = GetMutableStatByTag(Tag))
	{
		if (!bBroadcastStatChanged)
		{
			Stat->RemoveModifier(ModifierIndex);
		}
		else
		{
			const FSGStatValues OldValues = Stat->Values;
			Stat->RemoveModifier(ModifierIndex);
			const FSGStatValues NewValues = Stat->Values;
			if (!OldValues.Equals(NewValues))
			{
				HandleStatChanged(*Stat, OldValues, NewValues);
			}
		}
	}

	checkf(false, TEXT("Unable to get stat with tag %s"), *Tag.ToString());
}

void USGStatComponent::GetLevelRequirements(bool bRecomputeLevels, int32& OutLevelMin, int32& OutLevelMax, float& OutLevelPercent)
{
	if (CachedXPSumArray.IsEmpty())
	{
		InitializeXPCurve();
	}
	
	if (bRecomputeLevels)
	{
		RecomputeLevel();
	}

	const int32 Level = LevelCached;

	if (Level == SG_MAX_LEVEL)
	{
		OutLevelMin = CachedXPSumArray[SG_MAX_LEVEL];
		OutLevelMax = CachedXPSumArray[SG_MAX_LEVEL];
		OutLevelPercent = 0;
	}
	else
	{
		OutLevelMin = CachedXPSumArray[Level];
		OutLevelMax = CachedXPSumArray[Level + 1];

		const float CurrentXP = GetXPStat()->GetCurrentValue();
		OutLevelPercent = (CurrentXP - OutLevelMin) / (OutLevelMax - OutLevelMin);
	}
}

const FSGStat* USGStatComponent::GetStatByTag(const FGameplayTag& Tag) const
{
	return Stats.Find(Tag);
}

void USGStatComponent::ApplyDamageToSelf(const FSGDamageParams& DamageParams)
{
	const FGameplayTag HealthTag = GetHealthTag();
	check(DamageParams.bBaseDamageCalculated);
	float RequestedFlatDamage = DamageParams.FinalFlatDamage;
	float RequestedAttackScaling = DamageParams.FinalAttackScaling;

	// Temp Math
	check(DamageParams.DamageOwnerActor.IsValid());

	float Attack = 1.0f;
	if (DamageParams.DamageOwnerActor.IsValid())
	{
		if (USGStatComponent* EnemyStatComponent = DamageParams.DamageOwnerActor->FindComponentByClass<USGStatComponent>())
		{
			Attack = EnemyStatComponent->GetAttackStat()->GetCurrentValue();
		}
	}

	const float Defence = GetStatValue(GetDefenceTag());
	RequestedFlatDamage = FMath::Max((RequestedFlatDamage + (RequestedAttackScaling * Attack)) - Defence, 1);
	
	FSGStatModifier HealthModifier(HealthTag, -RequestedFlatDamage, 0, 0);
	HealthModifier.ModiferType = ESGStatModifierType::Additive;
	HealthModifier.EffectDurationType = ESGEffectDurationType::Instantaneous;

	AddStatModifier(HealthTag, HealthModifier);
	if (ASGCharacterBase* CharacterBase = Cast<ASGCharacterBase>(DamageParams.DamageTarget.Get()))
	{
		CharacterBase->HandleSGDamage(DamageParams, RequestedFlatDamage, GetHealthStat()->GetCurrentValue());
	}
}

FSGStat* USGStatComponent::GetMutableStatByTag(const FGameplayTag& Tag)
{
	return Stats.Find(Tag);
}

FSGStat* USGStatComponent::GetHealthStat() const
{
	return HealthStat;
}

FSGStat* USGStatComponent::GetDefenceStat() const
{
	return DefenceStat;
}

FSGStat* USGStatComponent::GetAttackStat() const
{
	return AttackStat;
}

FSGStat* USGStatComponent::GetXPStat() const
{
	return XPStat;
}

FGameplayTag USGStatComponent::GetXPTag()
{
	return UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Stat.XP"));
}

void USGStatComponent::RecomputeLevel()
{
	if (XPCurve == nullptr)
	{
		return;
	}
	
	if (CachedXPSumArray.IsEmpty())
	{
		InitializeXPCurve();
	}
	
	check(CachedXPSumArray.IsValidIndex(SG_MAX_LEVEL));

	int32 CurrentLevel = 1;

	const int32 CurrentXP = GetXPStat()->GetCurrentValue();

	if (CurrentXP >= CachedXPSumArray[SG_MAX_LEVEL])
	{
		CurrentLevel = SG_MAX_LEVEL;
	}
	else
	{
		for (int32 Level = 1; Level < SG_MAX_LEVEL; Level++)
		{
			if (CurrentXP < CachedXPSumArray[Level + 1])
			{
				CurrentLevel = Level;
				break;
			}
		}
	}

	if (CurrentLevel != LevelCached)
	{
		LevelCached = CurrentLevel;
		
		ApplyBaseStatEffects(CurrentLevel);
		OnLevelChanged.Broadcast(CurrentLevel);
	}
}

FGameplayTag USGStatComponent::GetHealthTag()
{
	return UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Stat.Health"));
}

FGameplayTag USGStatComponent::GetAttackTag()
{
	return UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Stat.Attack"));
}

FGameplayTag USGStatComponent::GetDefenceTag()
{
	return UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Stat.Defence"));
}

void USGStatComponent::InitializeStats()
{
	// Initialize
	// Note: These default values really shouldn't matter - They should be reset by a modifier anyway
	const FGameplayTag HealthTag = GetHealthTag();
	if (!Stats.Contains(HealthTag))
	{
		Stats.Add(HealthTag, FSGStat(HealthTag, 100.0f, 0, 100.0f));
	}

	HealthStat = Stats.Find(HealthTag);
	
	const FGameplayTag AttackTag = GetAttackTag();
	if (!Stats.Contains(AttackTag))
	{
		Stats.Add(AttackTag, FSGStat(AttackTag, 1.0f, 0, TNumericLimits<float>::Max()));
	}

	AttackStat = Stats.Find(AttackTag);
	
	const FGameplayTag DefenceTag = GetDefenceTag();
	if (!Stats.Contains(DefenceTag))
	{
		Stats.Add(DefenceTag, FSGStat(DefenceTag, 0.0f, 0, TNumericLimits<float>::Max()));
	}

	DefenceStat = Stats.Find(DefenceTag);

	const FGameplayTag XPTag = GetXPTag();
	if (!Stats.Contains(XPTag))
	{
		Stats.Add(XPTag, FSGStat(XPTag, 0.0f, 0, TNumericLimits<float>::Max()));
	}

	XPStat = Stats.Find(XPTag);
	
}

void USGStatComponent::HandleStatChanged(FSGStat& Stat, const FSGStatValues& OldStat, const FSGStatValues& NewStat)
{
	if (Stat.Tag == GetXPTag())
	{
		RecomputeLevel();
	}
	
	OnStatChanged.Broadcast(Stat.Tag, OldStat, NewStat);
}

void USGStatComponent::InitializeXPCurve()
{
	if (XPCurve != nullptr)
	{
		CachedXPSumArray.SetNum(SG_MAX_LEVEL + 2);
		CachedXPSumArray[0] = 0;
		CachedXPSumArray[1] = 0;

		int32 RunningSum = 0;
		// CachedXPSumArray[i] = The xp you need to get to level i
		for (int32 Level = 2; Level <= SG_MAX_LEVEL; Level++)
		{
			RunningSum += XPCurve->GetFloatValue(Level);
			CachedXPSumArray[Level] = RunningSum;
		}

		XPStat->SetBaseMaxValue(CachedXPSumArray[SG_MAX_LEVEL]);
	}
}

void USGStatComponent::ApplyBaseStatEffects(int32 Level)
{
	if (USGAbilityComponent* AbilityComponent = this->GetOwner()->FindComponentByClass<USGAbilityComponent>())
	{
		for (const auto& EffectClass : BaseStatEffects)
		{
			if (const USGEffect* EffectCDO = Cast<USGEffect>(EffectClass->GetDefaultObject()))
			{
				check(EffectCDO->GetEffectDurationType() == ESGEffectDurationType::Instantaneous);
			
				FSGEffectInstigatorContext InstigatorContext;
				InstigatorContext.EffectClass = EffectClass;
				InstigatorContext.OwnerActor = this->GetOwner();
				InstigatorContext.AvatarActor = this->GetOwner();
				InstigatorContext.Level = Level;

				FSGEffectEvaluationContext EvaluationContext;
				EvaluationContext.TargetActor = GetOwner();

				USGEffect* EffectInstance = USGEffect::NewSGEffect(this, InstigatorContext);
				AbilityComponent->GiveEffect(EffectInstance, EvaluationContext);
			}
		}
	}
}

float FSGStatModifier::GetBaseValueModifier() const
{
	if (BaseValueModifierCurve != nullptr)
	{
		return BaseValueModifierCurve->GetFloatValue(Level);
	}

	return BaseValueModifier;
}

float FSGStatModifier::GetMinValueModifier() const
{
	if (MinValueModifierCurve != nullptr)
	{
		return MinValueModifierCurve->GetFloatValue(Level);
	}

	return MinValueModifier;
}

float FSGStatModifier::GetMaxValueModifier() const
{
	if (MaxValueModifierCurve != nullptr)
	{
		return MaxValueModifierCurve->GetFloatValue(Level);
	}

	return MaxValueModifier;
}

void FSGStat::SetBaseValue(float Value)
{
	Values.Value.BaseValue = FMath::Clamp(Value, Values.MinValue.BaseValue, Values.MaxValue.BaseValue);

	ComputeCurrentValue();
}


void FSGStat::SetBaseMaxValue(float Value)
{
	Values.MaxValue.BaseValue = Value;

	ComputeCurrentValue();
}

void FSGStat::SetBaseMinValue(float Value)
{
	Values.MinValue.BaseValue = Value;

	ComputeCurrentValue();
}

void FSGStat::ComputeCurrentValue()
{
	float AdditiveBase = 0;
	float AdditiveMin = 0;
	float AdditiveMax = 0;

	float MultiplicativeBase = 0;
	float MultiplicativeMin = 0;
	float MultiplicativeMax = 0;
	for (const TPair<int32, FSGStatModifier>& Modifier : m_StatModifiers)
	{
		if (Modifier.Value.ModiferType == ESGStatModifierType::Additive)
		{
			Modifier.Value.AddValues(AdditiveBase, AdditiveMin, AdditiveMax);
		}
		else if (Modifier.Value.ModiferType == ESGStatModifierType::Multiplicative)
		{
			Modifier.Value.AddValues(MultiplicativeBase, MultiplicativeMin, MultiplicativeMax);
		}
	}

	Values.Value.CurrentValue = Values.Value.BaseValue * (1.0f + MultiplicativeBase) + AdditiveBase;
	Values.MinValue.CurrentValue = Values.MinValue.BaseValue * (1.0f + MultiplicativeMin) + AdditiveMin;
	Values.MaxValue.CurrentValue = Values.MaxValue.BaseValue * (1.0f + MultiplicativeMax) + AdditiveMax;
}

int32 FSGStat::AddModifier(const FSGStatModifier& Modifier)
{
	// Set base modifiers are immediate and irreversable.
	if (Modifier.EffectDurationType == ESGEffectDurationType::Instantaneous)
	{
		if (Modifier.AffectMinValue)
		{
			switch (Modifier.ModiferType)
			{
				case ESGStatModifierType::Set:
					SetBaseMinValue(Modifier.GetMinValueModifier());
					break;
				case ESGStatModifierType::Additive:
					SetBaseMinValue(Values.MinValue.BaseValue + Modifier.GetMinValueModifier());
					break;
				case ESGStatModifierType::Multiplicative:
					SetBaseMinValue(Values.MinValue.BaseValue * Modifier.GetMinValueModifier());	
			}
		}

		if (Modifier.AffectMaxValue)
		{
			switch (Modifier.ModiferType)
			{
			case ESGStatModifierType::Set:
				SetBaseMaxValue(Modifier.GetMaxValueModifier());
				break;
			case ESGStatModifierType::Additive:
				SetBaseMaxValue(Values.MaxValue.BaseValue + Modifier.GetMaxValueModifier());
				break;
			case ESGStatModifierType::Multiplicative:
				SetBaseMaxValue(Values.MaxValue.BaseValue * Modifier.GetMaxValueModifier());	
			}
		}

		if (Modifier.AffectBaseValue)
		{
			switch (Modifier.ModiferType)
			{
			case ESGStatModifierType::Set:
				SetBaseValue(Modifier.GetBaseValueModifier());
				break;
			case ESGStatModifierType::Additive:
				SetBaseValue(Values.Value.BaseValue + Modifier.GetBaseValueModifier());
				break;
			case ESGStatModifierType::Multiplicative:
				SetBaseValue(Values.Value.BaseValue * Modifier.GetBaseValueModifier());	
			}
		}
		
		return INDEX_NONE;
	}
	
	const int32 Handle = m_CurrentModifierKey++;
	m_StatModifiers.Add(Handle, Modifier);
	ComputeCurrentValue();
	return Handle;
}

void FSGStat::RemoveModifier(int32 Handle)
{
	if (m_StatModifiers.Contains(Handle))
	{
		m_StatModifiers.Remove(Handle);
		ComputeCurrentValue();
	}
}

