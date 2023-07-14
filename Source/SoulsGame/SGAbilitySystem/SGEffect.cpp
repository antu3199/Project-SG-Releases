#include "SGEffect.h"

#include "SGAbilityComponent.h"

USGAbilityComponent* FSGEffectInstigatorContext::GetAbilityComponent() const
{
	if (!OwnerActor.IsValid())
	{
		return nullptr;
	}
	
	if (USGAbilityComponent* AbilityComponent = OwnerActor->FindComponentByClass<USGAbilityComponent>())
	{
		return AbilityComponent;
	}

	return nullptr;
}

USGEffect::USGEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

USGEffect* USGEffect::NewSGEffect(UObject* Outer, const FSGEffectInstigatorContext& Context)
{
	ensure(Outer);
	ensure(Context.EffectClass);
	ensure(Context.IsValid());
	ensure(Context.GetAbilityComponent());
	
	USGEffect* NewObj = NewObject<USGEffect>(Outer, Context.EffectClass);
	NewObj->bIsCDO = false;
	NewObj->InitEffect(Context);

	return NewObj;
}

void USGEffect::InitEffect(const FSGEffectInstigatorContext& Context)
{
	EffectContext = Context;
	EffectContext->EffectInstance = MakeWeakObjectPtr(this);
	SetLevel(Context.Level);
}

void USGEffect::RequestGiveEffect(FSGEffectEvaluationContext InEvaluationContext, bool bApplyEffectOnGive)
{
	check(InEvaluationContext.TargetActor.IsValid() && InEvaluationContext.TargetActor->FindComponentByClass<USGAbilityComponent>());
	if (bIsStackable)
	{
		if (NumStacks > 0)
		{
			const int32 OldStacks = NumStacks;
			NumStacks++;
			const int32 NewStacks = NumStacks;

			OnStackChanged(OldStacks, NewStacks);
			return;
		}
		else
		{
			NumStacks = 1;
		}
	}
	
	EvaluationContext = InEvaluationContext;

	OnGiveEffect();

	if (EffectDurationType == ESGEffectDurationType::Instantaneous || bApplyEffectOnGive)
	{
		RequestApplyEffect();
	}
}

void USGEffect::EndEffect()
{
	check(EffectContext.IsSet() && EvaluationContext.IsSet());

	OnEndEffect();


	if (USGAbilityComponent* AbilityComponent = EffectContext->GetAbilityComponent())
	{
		AbilityComponent->OnEffectInstanceEnded(this);
	}
}

bool USGEffect::RequestApplyEffect()
{
	check(EffectContext.IsSet() && EvaluationContext.IsSet());

	if (!QueryCanApplyEffect())
	{
		return false;
	}
	
	const bool bResult = OnApplyEffect();
	if (bResult)
	{
		if (EffectDurationType == ESGEffectDurationType::Instantaneous)
		{
			EndEffect();
		}
	}
	return bResult;
}

void USGEffect::RequestEndEffect()
{
	check(EffectDurationType != ESGEffectDurationType::Instantaneous);

	if (bIsStackable)
	{
		const int32 OldStacks = NumStacks;
		NumStacks--;
		const int32 NewStacks = NumStacks;

		OnStackChanged(OldStacks, NewStacks);
		if (NumStacks <= 0)
		{
			EndEffect();
		}
	}
	else
	{
		EndEffect();
	}
}

void USGEffect::ForceEndEffect()
{
	check(EffectDurationType != ESGEffectDurationType::Instantaneous);

	EndEffect();
}

bool USGEffect::QueryCanApplyEffect_Implementation()
{
	return true;
}

void USGEffect::AddStatModifier(FSGStatModifier Modifier)
{
	FSGStatModifier& NewModifier = StatModifiers.Add_GetRef(Modifier);
	NewModifier.SetLevel(Level);
}

void USGEffect::SetLevel(int32 InLevel)
{
	for (auto& Modifier : StatModifiers)
	{
		Modifier.SetLevel(InLevel);
	}

	Level = InLevel;
}

int32 USGEffect::GetLevel() const
{
	return Level;
}


FSGDamageParams USGEffect::OnPreSelfDamage_Implementation(const FSGDamageParams& InDamageParams)
{
	check(EffectDurationType == ESGEffectDurationType::StatusEffect);
	return InDamageParams;
}

void USGEffect::OnPostSelfDamage_Implementation(const FSGDamageParams& InDamageParams)
{
	check(EffectDurationType == ESGEffectDurationType::StatusEffect);
}

FSGDamageParams USGEffect::OnPreDealDamage_Implementation(const FSGDamageParams& InDamageParams)
{
	check(EffectDurationType == ESGEffectDurationType::StatusEffect);
	return InDamageParams;
}

void USGEffect::OnPostDealDamage_Implementation(const FSGDamageParams& InDamageParams)
{
	check(EffectDurationType == ESGEffectDurationType::StatusEffect);
}

void USGEffect::SetModifiersFromParams(const FSGStatModifierEffectParams Params)
{
	for (auto& Modifier : Params.StatModifiers)
	{
		AddStatModifier(Modifier);
	}
}

void USGEffect::OnGiveEffect_Implementation()
{
	check(EffectContext.IsSet() && EvaluationContext.IsSet());
}

bool USGEffect::OnApplyEffect_Implementation()
{
	check(EffectContext.IsSet() && EvaluationContext.IsSet());

	if (!EvaluationContext->TargetActor.IsValid())
	{
		return false;
	}

	if (USGStatComponent* StatComponent = EvaluationContext->TargetActor->FindComponentByClass<USGStatComponent>())
	{
		for (const FSGStatModifier& Modifier : StatModifiers)
		{
			checkf(EffectDurationType == Modifier.EffectDurationType, TEXT("Effect must last as long as the stat application %s"), *GetNameSafe(this))
			int32 Handle = StatComponent->AddStatModifier(Modifier.StatTag, Modifier);
			if (EffectDurationType == ESGEffectDurationType::StatusEffect)
			{
				ModifierHandles.Add(Modifier.StatTag, Handle);
			}
		}
	}

	return true;
}

void USGEffect::OnStackChanged_Implementation(int32 OldStacks, int32 NewStacks)
{
	check(EffectContext.IsSet() && EvaluationContext.IsSet());
}

void USGEffect::OnEndEffect_Implementation()
{
	check(EffectContext.IsSet() && EvaluationContext.IsSet());
	
	if (USGStatComponent* StatComponent = EvaluationContext->TargetActor->FindComponentByClass<USGStatComponent>())
	{
		for (const TPair<FGameplayTag, int32>& ModifierPair : ModifierHandles)
		{
			StatComponent->RemoveStatModifier(ModifierPair.Key, ModifierPair.Value);
		}
	}

	ModifierHandles.Reset();
}

