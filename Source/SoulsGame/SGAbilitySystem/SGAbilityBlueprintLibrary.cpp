#include "SGAbilityBlueprintLibrary.h"

#include "AbilitySystemComponent.h"
#include "SGAbilityComponent.h"

USGAbilitySystemBlueprintLibrary::USGAbilitySystemBlueprintLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

USGAbilityComponent* USGAbilitySystemBlueprintLibrary::GetAbilityComponent(AActor* Actor)
{
	if (USGAbilityComponent* Component = Actor->FindComponentByClass<USGAbilityComponent>())
	{
		return Component;
	}

	return nullptr;
}

void USGAbilitySystemBlueprintLibrary::SendAbilityEventToActor(AActor* Actor, FGameplayTag EventTag,
	FSGAbilityEventData Payload)
{
	if (USGAbilityComponent* Component = GetAbilityComponent(Actor))
	{
		Payload.EventTag = EventTag;
		Payload.MontageInstance = GetMontageInstanceFromActor(Actor);
		Component->HandleAbilityEvent(&Payload);
	}
}

USGEffect* USGAbilitySystemBlueprintLibrary::StaticCreateEffect(
	FSGEffectInstigatorContext InstigatorContext)
{
	USGEffect* Effect = USGEffect::NewSGEffect(InstigatorContext.OwnerActor.Get(), InstigatorContext);
	return Effect;
}

USGEffect* USGAbilitySystemBlueprintLibrary::StaticGiveEffect(USGEffect* EffectInstance, FSGEffectEvaluationContext EvaluationContext)
{
	if (EffectInstance == nullptr)
	{
		return nullptr;
	}

	check(EffectInstance->GetInstigatorContext().IsValid());

	if (USGAbilityComponent* AbilityComponent = EffectInstance->GetInstigatorContext().GetAbilityComponent())
	{
		return AbilityComponent->GiveEffect(EffectInstance, EvaluationContext);
	}
	
	return nullptr;
}

USGEffect* USGAbilitySystemBlueprintLibrary::StaticCreateModifierEffect(
	FSGEffectInstigatorContext InstigatorContext, FSGStatModifierEffectParams StatModifierEffectParams)
{
	USGEffect* Effect = StaticCreateEffect(InstigatorContext);
	Effect->SetModifiersFromParams(StatModifierEffectParams);
	return Effect;
}

void USGAbilitySystemBlueprintLibrary::StaticApplyDamage(FSGDamageParams DamageParams)
{
	if (DamageParams.DamageTarget.IsValid())
	{
		checkf(DamageParams.DamageTarget->FindComponentByClass<USGStatComponent>(), TEXT("Target should have a stat component!"));

		// Perform initial calculation from assets
		DamageParams.CalculateBaseDamage();

		static const FGameplayTag PreSelfDamage = UGameplayTagsManager::Get().RequestGameplayTag(FName("Effect.ApplicationType.OnPreSelfDamage"), false);
		if (USGAbilityComponent* AbilityComponent = DamageParams.DamageTarget->FindComponentByClass<USGAbilityComponent>())
		{
			TArray<USGEffect*> Effects;
			AbilityComponent->GetEffectsWithApplicationType(PreSelfDamage, Effects);
			for (USGEffect* Effect : Effects)
			{
				DamageParams = Effect->OnPreSelfDamage(DamageParams);
			}
		}

		static const FGameplayTag PreDealDamage = UGameplayTagsManager::Get().RequestGameplayTag(FName("Effect.ApplicationType.OnPreDealDamage"), false);
		if (USGAbilityComponent* AbilityComponent = DamageParams.DamageOwnerActor->FindComponentByClass<USGAbilityComponent>())
		{
			TArray<USGEffect*> Effects;
			AbilityComponent->GetEffectsWithApplicationType(PreDealDamage, Effects);
			for (USGEffect* Effect : Effects)
			{
				DamageParams = Effect->OnPreDealDamage(DamageParams);
			}
		}
		
		static const FGameplayTag UntargettableTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Effect.Shared.Untargettable"), false);
		if (DamageParams.Modifiers.HasTag(UntargettableTag))
		{
			return;
		}
		
		if (USGStatComponent* StatComponent = DamageParams.DamageTarget->FindComponentByClass<USGStatComponent>())
		{
			StatComponent->ApplyDamageToSelf(DamageParams);
		}

		static const FGameplayTag PostSelfDamage = UGameplayTagsManager::Get().RequestGameplayTag(FName("Effect.ApplicationType.OnPostSelfDamage"), false);
		if (USGAbilityComponent* AbilityComponent = DamageParams.DamageTarget->FindComponentByClass<USGAbilityComponent>())
		{
			TArray<USGEffect*> Effects;
			AbilityComponent->GetEffectsWithApplicationType(PostSelfDamage, Effects);
			for (USGEffect* Effect : Effects)
			{
				Effect->OnPostSelfDamage(DamageParams);
			}
		}

		static const FGameplayTag PostDealDamage = UGameplayTagsManager::Get().RequestGameplayTag(FName("Effect.ApplicationType.OnPostDealDamage"), false);
		if (USGAbilityComponent* AbilityComponent = DamageParams.DamageOwnerActor->FindComponentByClass<USGAbilityComponent>())
		{
			TArray<USGEffect*> Effects;
			AbilityComponent->GetEffectsWithApplicationType(PostDealDamage, Effects);
			for (USGEffect* Effect : Effects)
			{
				Effect->OnPostDealDamage(DamageParams);
			}
		}
	}
}

FAnimMontageInstance* USGAbilitySystemBlueprintLibrary::GetMontageInstanceFromActor(const AActor* Actor)
{
	if (const USkeletalMeshComponent* MeshComponent = Actor->FindComponentByClass<USkeletalMeshComponent>())
	{
		if (const UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
		{
			if (FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveMontageInstance())
			{
				return MontageInstance;
			}
		}
	}

	return nullptr;
}
