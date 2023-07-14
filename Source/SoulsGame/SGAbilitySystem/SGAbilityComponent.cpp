#include "SGAbilityComponent.h"

#include "SGAbility.h"

FGameplayTag FSGAbilitySpec::GetTag() const
{
	if (const USGAbility* AbilityCDO = Cast<USGAbility>(AbilityClass->GetDefaultObject()))
	{
		return AbilityCDO->GetIdentifierTag();
	}

	return FGameplayTag();
}

USGAbilityComponent::USGAbilityComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USGAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	for (const auto& AbilityClass : DefaultGivenAbilities)
	{
		// TODO: Do I need to look at level here?
		GiveAbility(AbilityClass);
	}

	for (const auto& EffectClass : DefaultGivenEffects)
	{
		if (const USGEffect* EffectCDO = Cast<USGEffect>(EffectClass->GetDefaultObject()))
		{
			FSGEffectInstigatorContext InstigatorContext;
			InstigatorContext.EffectClass = EffectClass;
			InstigatorContext.OwnerActor = this->GetOwner();
			InstigatorContext.AvatarActor = this->GetOwner();
			InstigatorContext.Level = EffectCDO->GetLevel();

			FSGEffectEvaluationContext EvaluationContext;
			EvaluationContext.TargetActor = GetOwner();

			USGEffect* EffectInstance = USGEffect::NewSGEffect(this, InstigatorContext);
			GiveEffect(EffectInstance, EvaluationContext);
		}
	}

	if (USGStatComponent* StatComponent = GetOwner()->FindComponentByClass<USGStatComponent>())
	{
		FSGStat* Stat = StatComponent->GetHealthStat();
		float Test = Stat->GetCurrentValue();
		int32 ASD = 0;
	}
}

void USGAbilityComponent::GiveAbility(TSubclassOf<USGAbility> AbilityClass, int32 Level)
{
	check(AbilityClass);

	if (!AbilityClass)
	{
		return;
	}

	const FGameplayTag Tag = GetTagFromAbilityClass(AbilityClass);

	if (!Tag.IsValid())
	{
		return;
	}

	FSGAbilitySpec& AbilitySpec = GivenAbilities.FindOrAdd(Tag);
	AbilitySpec.AbilityClass = AbilityClass;
	AbilitySpec.Level = Level;

	if (const USGAbility* AbilityCDO = Cast<USGAbility>(AbilityClass->GetDefaultObject()))
	{
		// If we don't create instance on activation, then create instance here!
		if (!AbilityCDO->CreateInstanceOnActivation)
		{
			AbilitySpec.bIsInstancedOnActivation = AbilityCDO->CreateInstanceOnActivation;
			CreateAbilityInstance(AbilitySpec);
		}
	}
}

int32 USGAbilityComponent::RequestActivateAbilityByTag(const FGameplayTag& Tag, AActor* AvatarActor)
{
	const FSGAbilitySpec* Spec = GetGivenAbilitySpecByTag(Tag);
	if (!Spec)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to request ability that wasn't given!"));
		return INDEX_NONE;
	}

	return RequestActivateAbilityBySpec(Spec);
}

int32 USGAbilityComponent::RequestActivateAbilityBySpec(const FSGAbilitySpec* Spec, AActor* AvatarActor)
{
	FGameplayTag Tag;
	if (USGAbility* AbilityCDO = Cast<USGAbility>(Spec->AbilityClass->GetDefaultObject()))
	{
		Tag = AbilityCDO->GetIdentifierTag();
		if (!AbilityCDO->CanActivate(this))
		{
			return INDEX_NONE;
		}
	}

	check(Tag.IsValid());

	// Ability already exists
	if (!Spec->bIsInstancedOnActivation)
	{
		if (USGAbility* Ability = GetAbilityInstanceByTag(Tag))
		{
			// Set ability level
			if (Ability->GetLevel() != Spec->Level)
			{
				Ability->SetLevel(Spec->Level);
			}
	
			check(!Ability->IsActive());
			Ability->RequestActivate();
	
			return Ability->InstancedAbilityHandle;
			// else
			// {
			// 	return INDEX_NONE;
			// 	//check(false);
			// }
		}
	}
	
	USGAbility* InstancedAbility = nullptr;

	// if (!Spec->bIsInstancedOnActivation)
	// {
	// 	InstancedAbility = GetAbilityInstanceByTag(Tag);
	// }
	
	int32 InstancedHandle = INDEX_NONE;

	if (InstancedAbility == nullptr)
	{
		InstancedHandle = CreateAbilityInstance(*Spec);
		check(AbilityInstances.Contains(InstancedHandle));

		const FSGAbilityInstanceWrapper& Wrapper = AbilityInstances[InstancedHandle];
		check(Wrapper.Ability != nullptr);

		InstancedAbility = AbilityInstances[InstancedHandle].Ability;
	}

	// Probably not needed but just in case:
	if (!InstancedAbility->CanActivate(this))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed activate: %s"), *Tag.ToString());
		return InstancedHandle;
	}

	const bool bResult = InstancedAbility->RequestActivate();
	check(bResult);
	check(InstancedHandle != INDEX_NONE);

	return InstancedHandle;
}

bool USGAbilityComponent::HasGivenAbility(const FGameplayTag& Tag) const
{
	return GivenAbilities.Contains(Tag);
}

void USGAbilityComponent::GetGivenAbilities(TMap<FGameplayTag, FSGAbilitySpec>& OutAbilities) const
{
	OutAbilities.Append(GivenAbilities);
}

FSGAbilitySpec* USGAbilityComponent::GetGivenAbilitySpecByTag(const FGameplayTag& Tag)
{
	if (!GivenAbilities.Contains(Tag))
	{
		return nullptr;
	}

	return GivenAbilities.Find(Tag);
}


USGAbility* USGAbilityComponent::GetAbilityInstanceById(int32 InstanceHandle) const
{
	if (!AbilityInstances.Contains(InstanceHandle))
	{
		return nullptr;
	}
	
	return AbilityInstances[InstanceHandle].Ability;
}

USGAbility* USGAbilityComponent::GetAbilityInstanceByTag(const FGameplayTag& Tag) const
{
	if (!AbilityTagToInstanceHandles.Contains(Tag))
	{
		return nullptr;
	}
	
	const TSet<int32>& Handles = AbilityTagToInstanceHandles[Tag];
	ensure(Handles.Num() == 1);

	const FSGAbilityInstanceWrapper* AbilityInstanceWrapper = AbilityInstances.Find(*Handles.begin());
	if (AbilityInstanceWrapper == nullptr)
	{
		return nullptr;
	}

	return AbilityInstanceWrapper->Ability;
}

void USGAbilityComponent::GetAbilityInstancesByTag(const FGameplayTag& Tag, TArray<USGAbility*>& OutAbilities) const
{
	if (!AbilityTagToInstanceHandles.Contains(Tag))
	{
		return;
	}

	const TSet<int32>& Handles = AbilityTagToInstanceHandles[Tag];
	for (const int32& Handle : Handles)
	{
		check(AbilityInstances.Contains(Handle));
		OutAbilities.Add(AbilityInstances[Handle].Ability);
	}
}

bool USGAbilityComponent::IsAbilityActive(const int32& AbilityInstanceHandle) const
{
	if (const USGAbility* AbilityInstance = GetAbilityInstanceById(AbilityInstanceHandle))
	{
		return AbilityInstance->IsActive();
	}

	return false;
}

bool USGAbilityComponent::IsAbilityActive(const FGameplayTag& Tag) const
{
	if (const USGAbility* AbilityInstance = GetAbilityInstanceByTag(Tag))
	{
		return AbilityInstance->IsActive();
	}

	return false;
}

void USGAbilityComponent::GetActivatableAbilities(TArray<FSGAbilitySpec>& OutAbilities) const
{
	for (const TPair<FGameplayTag, FSGAbilitySpec>& AbilityPair : GivenAbilities)
	{
		if (AbilityPair.Value.AbilityClass != nullptr)
		{
			if (USGAbility* Ability = Cast<USGAbility>(AbilityPair.Value.AbilityClass->GetDefaultObject()))
			{
				if (Ability->CanActivate(this))
				{
					OutAbilities.Add(AbilityPair.Value);
				}
			}
		}
	}
}

bool USGAbilityComponent::CanActivateAbilityWithTag(const FGameplayTag& Tag) const
{
	if (GivenAbilities.Contains(Tag))
	{
		if (USGAbility* Ability = Cast<USGAbility>(GivenAbilities[Tag].AbilityClass->GetDefaultObject()))
		{
			return Ability->CanActivate(this);
		}
	}

	return false;
}

void USGAbilityComponent::HandleAbilityEvent(const FSGAbilityEventData* Payload)
{
	if (Payload == nullptr)
	{
		return;
	}

	const bool bHasAbilityTag = Payload->AssociatedAbilityTag.IsValid();

	TArray<USGAbility*> Instances;

	if (bHasAbilityTag)
	{
		GetAbilityInstancesByTag(Payload->AssociatedAbilityTag, Instances);
	}
	else
	{
		for (const auto& Wrapper : AbilityInstances)
		{
			check(Wrapper.Value.Ability != nullptr);
			Instances.Add(Wrapper.Value.Ability);
		}
	}

	for (USGAbility* AbilityInstance : Instances)
	{
		if (AbilityInstance->IsActive())
		{
			bool bHandled = false;
			if (Payload->MontageInstance != nullptr)
			{
				if (AbilityInstance->GetCurrentMontage() == Payload->MontageInstance->Montage)
				{
					AbilityInstance->OnHandleEvent(*Payload);
					bHandled = true;
				}
			}
			else
			{
				AbilityInstance->OnHandleEvent(*Payload);
				bHandled = true;
			}

			if (bHandled && Payload->bBroadcastOnce)
			{
				return;
			}
			
		}
	}
}

void USGAbilityComponent::UnGiveAbilityByTag(const FGameplayTag& Tag)
{

	// Remove all active instances of the ability
	TArray<USGAbility*> Instances;

	GetAbilityInstancesByTag(Tag, Instances);

	for (const auto& AbilityInstance : Instances)
	{
		RemoveAbilityInstance(AbilityInstance);
	}
	
	const FSGAbilitySpec* Spec = GetGivenAbilitySpecByTag(Tag);
	if (!Spec)
	{
		return;
	}

	GivenAbilities.Remove(Tag);
}

void USGAbilityComponent::SetAbilityLevel(const FGameplayTag& AbilityTag, int32 Level)
{
	if (!GivenAbilities.Contains(AbilityTag))
	{
		return;
	}

	FSGAbilitySpec& Spec = GivenAbilities[AbilityTag];
	Spec.Level = Level;
}


FGameplayTag USGAbilityComponent::GetTagFromAbilityClass(const TSubclassOf<USGAbility>& AbilityClass)
{
	if (!AbilityClass)
	{
		return FGameplayTag();
	}
	
	if (const USGAbility* AbilityCDO = Cast<USGAbility>(AbilityClass->GetDefaultObject()))
	{
		return AbilityCDO->GetIdentifierTag();
	}

	return FGameplayTag();
}

USGEffect* USGAbilityComponent::CreateEffectInstance(const TSubclassOf<USGEffect>& EffectClass,
	const FSGEffectInstigatorContext& EffectContext)
{
	USGEffect* EffectInstance = USGEffect::NewSGEffect(this, EffectContext);
	return EffectInstance;
}

void USGAbilityComponent::GetEffectsWithApplicationType(const FGameplayTag& EffectApplicationType, TArray<USGEffect*>& OutEffects)
{
	for (const TPair<FGameplayTag, class USGEffect*>& EffectPair : ActiveStatusEffects)
	{
		if (EffectPair.Value)
		{
			const FGameplayTagContainer& ApplicationTypes = EffectPair.Value->GetAutomaticEffectApplicationTypes();
			if (ApplicationTypes.HasTag(EffectApplicationType))
			{
				check(EffectPair.Value->EffectDurationType == ESGEffectDurationType::StatusEffect);
				OutEffects.Add(EffectPair.Value);
			}
		}
	}
}

float USGAbilityComponent::GetCurrentCooldownTime(const FGameplayTag& Tag) const
{
	if (const FSGAbilitySpec* AbilitySpec = GivenAbilities.Find(Tag))
	{
		const float LastActivationTime = AbilitySpec->LastActivationTime;
		if (const USGAbility* AbilityCDO = AbilitySpec->AbilityClass.GetDefaultObject())
		{
			const float Cooldown = AbilityCDO->GetAbilityActivationCooldown();
			if (Cooldown == 0.0f || LastActivationTime == -1.0f)
			{
				return 0.0f;
			}

			if (const UWorld* World = GetWorld())
			{
				return FMath::Max(Cooldown - (World->GetTimeSeconds() - LastActivationTime), 0.0f);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[[USGAbilityComponent::GetCurrentCooldownTime] Unable to find ability CDO with tag %s"), *Tag.ToString());
			return 0.0f;
		}


		return 0.0f;
	}

	UE_LOG(LogTemp, Warning, TEXT("[[USGAbilityComponent::GetCurrentCooldownTime] Unable to find ability with tag %s"), *Tag.ToString());
	return -1;
}

float USGAbilityComponent::GetCurrentCooldownPercent(const FGameplayTag& Tag) const
{
	if (const USGAbility* Ability = GetAbilityInstanceByTag(Tag))
	{
		if (Ability->IsActive())
		{
			return 1.0f;
		}
	}
	
	if (const FSGAbilitySpec* AbilitySpec = GivenAbilities.Find(Tag))
	{
		const float LastActivationTime = AbilitySpec->LastActivationTime;
		if (const USGAbility* AbilityCDO = AbilitySpec->AbilityClass.GetDefaultObject())
		{
			
			const float ActivationCooldown = AbilityCDO->GetAbilityActivationCooldown();
			if (ActivationCooldown == 0.0f || LastActivationTime == -1.0f)
			{
				return 0.0f;
			}

			const float CooldownTime = GetCurrentCooldownTime(Tag);
			return CooldownTime / ActivationCooldown;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[USGAbilityComponent::GetCurrentCooldownPercent] Unable to find ability CDO with tag %s"), *Tag.ToString());
			return 0.0f;
		}


		return 0.0f;
	}

	UE_LOG(LogTemp, Warning, TEXT("[USGAbilityComponent::GetCurrentCooldownPercent] Unable to find ability with tag %s"), *Tag.ToString());
	return -1;
}

float USGAbilityComponent::GetLastActivationTime(const FGameplayTag& Tag) const
{
	if (const FSGAbilitySpec* AbilitySpec = GivenAbilities.Find(Tag))
	{
		return AbilitySpec->LastActivationTime;
	}

	UE_LOG(LogTemp, Warning, TEXT("[USGAbilityComponent::GetLastActivationTime] Unable to find ability with tag %s"), *Tag.ToString());
	return -1;
}

void USGAbilityComponent::RequestCooldownStart(const FGameplayTag& Tag)
{
	if (FSGAbilitySpec* AbilitySpec = GivenAbilities.Find(Tag))
	{
		AbilitySpec->LastActivationTime = GetWorld()->GetTimeSeconds();
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[USGAbilityComponent::GetLastActivationTime] Unable to find ability with tag %s"), *Tag.ToString());
}

bool USGAbilityComponent::IsUnderCooldown(const FGameplayTag& Tag) const
{
	if (const FSGAbilitySpec* AbilitySpec = GivenAbilities.Find(Tag))
	{
		if (const USGAbility* AbilityCDO = AbilitySpec->AbilityClass.GetDefaultObject())
		{
			const float Cooldown = AbilityCDO->GetAbilityActivationCooldown();
			if (Cooldown == 0.0f)
			{
				return false;
			}

			return GetCurrentCooldownTime(Tag) > 0;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[USGAbilityComponent::IsUnderCooldown] Unable to find ability CDO with tag %s"), *Tag.ToString());
			return false;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[USGAbilityComponent::IsUnderCooldown] Unable to find ability with tag %s"), *Tag.ToString());
	return false;
}

bool USGAbilityComponent::HasActiveEffect(const FGameplayTag& Tag) const
{
	if (USGEffect* Effect = GetActiveEffect(Tag))
	{
		return true;
	}

	return false;
}

USGEffect* USGAbilityComponent::GetActiveEffect(const FGameplayTag& Tag) const
{
	if (USGEffect* const* EffectPtr = ActiveStatusEffects.Find(Tag))
	{
		return *EffectPtr;
	}

	return nullptr;
}

void USGAbilityComponent::RemoveEffectByTag(const FGameplayTag& Tag)
{
	ActiveStatusEffects.Remove(Tag);
}

void USGAbilityComponent::RemoveEffectByInstance(USGEffect* Effect)
{
	FGameplayTag TagToRemove;
	for (const TPair<FGameplayTag, class USGEffect*> & Pair : ActiveStatusEffects)
	{
		if (Pair.Value == Effect)
		{
			TagToRemove = Pair.Key;
			break;
		}
	}

	if (TagToRemove.IsValid())
	{
		ActiveStatusEffects.Remove(TagToRemove);
	}
}

USGEffect* USGAbilityComponent::GiveEffect(USGEffect* EffectInstance,
                                           const FSGEffectEvaluationContext& EffectEvaluationContext)
{
	check(EffectInstance);

	const FGameplayTag Tag = EffectInstance->GetIdentifierTag();
	if (ActiveStatusEffects.Contains(Tag))
	{
		USGEffect* ExistingEffectInstance = ActiveStatusEffects[Tag];
		check(IsValid(ExistingEffectInstance));
		check(ExistingEffectInstance->GetClass() == EffectInstance->GetClass());

		ExistingEffectInstance->RequestGiveEffect(EffectEvaluationContext);

		// An effect that hasn't been given is garbage anyway
		if (EffectInstance != ExistingEffectInstance)
		{
			EffectInstance->MarkAsGarbage();
		}
		return ExistingEffectInstance;
	}

	check(!ActiveStatusEffects.Contains(Tag));
	
	EffectInstance->RequestGiveEffect(EffectEvaluationContext);

	if (EffectInstance->GetEffectDurationType() != ESGEffectDurationType::Instantaneous)
	{
		ActiveStatusEffects.Add(EffectInstance->GetIdentifierTag(), EffectInstance);
	}

	return EffectInstance;
}


void USGAbilityComponent::OnAbilityInstanceEnded(USGAbility* AbilityInstance)
{
	if (!AbilityInstance->GetCreateInstancedOnActivation())
	{
		// If we don't create instances on activation, do nothing!
	}
	else
	{
		RemoveAbilityInstance(AbilityInstance);
	}
}

void USGAbilityComponent::OnEffectInstanceEnded(USGEffect* EffectInstance)
{
	if (!EffectInstance)
	{
		return;
	}

	if (EffectInstance->GetEffectDurationType() == ESGEffectDurationType::Instantaneous)
	{
		return;
	}

	const FGameplayTag Tag = EffectInstance->GetIdentifierTag();
	check(ActiveStatusEffects.Contains(Tag));

	ActiveStatusEffects.Remove(Tag);
	EffectInstance->MarkAsGarbage();
}

void USGAbilityComponent::RemoveAbilityInstance(USGAbility* AbilityInstance)
{
	if (!AbilityInstance)
	{
		return;
	}

	if (AbilityInstance->IsActive())
	{
		AbilityInstance->RequestEndAbility();
	}

	// If we do, then we have to remove it from the ability instances
	const int32 InstanceHandle = AbilityInstance->InstancedAbilityHandle;
	check(InstanceHandle != FSGHandleGenerator::Handle_Invalid);
	AbilityInstances.Remove(InstanceHandle);

	const FGameplayTag& Tag = AbilityInstance->GetIdentifierTag();
	check(AbilityTagToInstanceHandles.Contains(Tag));
	TSet<int32>& HandleSet = AbilityTagToInstanceHandles[Tag];
	check(HandleSet.Contains(InstanceHandle));
	HandleSet.Remove(InstanceHandle);

	if (HandleSet.Num() == 0)
	{
		AbilityTagToInstanceHandles.Remove(Tag);
	}
	
	AbilityInstance->MarkAsGarbage();

	AbilityInstanceHandleGenerator.FreeHandle(InstanceHandle);
}

int32 USGAbilityComponent::CreateAbilityInstance(const FSGAbilitySpec& AbilitySpec)
{
	if (!AbilitySpec.AbilityClass)
	{
		return FSGHandleGenerator::Handle_Invalid;
	}

	const FGameplayTag Tag = GetTagFromAbilityClass(AbilitySpec.AbilityClass);

	if (!Tag.IsValid())
	{
		return FSGHandleGenerator::Handle_Invalid;
	}
	const int32 Handle = AbilityInstanceHandleGenerator.GetNewHandle();
	check(!AbilityInstances.Contains(Handle));
	check(Handle != FSGHandleGenerator::Handle_Invalid);
	
	FSGAbilityInstanceWrapper& InstancedAbilityWrapper = AbilityInstances.FindOrAdd(Handle);
	USGAbility* InstancedAbility = USGAbility::NewSGAbility(this, AbilitySpec.AbilityClass, Handle, GetOwner(), nullptr, AbilitySpec.Level);
	InstancedAbilityWrapper.Handle = Handle;
	InstancedAbilityWrapper.Ability = InstancedAbility;

	TSet<int32>& HandleSet = AbilityTagToInstanceHandles.FindOrAdd(Tag);
	HandleSet.Add(Handle);

	return Handle;
}



