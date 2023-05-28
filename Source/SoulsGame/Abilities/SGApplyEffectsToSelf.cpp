// Fill out your copyright notice in the Description page of Project Settings.


#include "SGApplyEffectsToSelf.h"

#include "SoulsGame/Character/SCharacterBase.h"


void USGApplyEffectsToSelf::PreActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::PreActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void USGApplyEffectsToSelf::DoActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::DoActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASCharacterBase* Character = Cast<ASCharacterBase>(GetActorInfo().OwnerActor);

	
	UAbilitySystemComponent* AbilitySystemComponent = Character->GetAbilitySystemComponent();
	EffectRemovedHandle = AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &USGApplyEffectsToSelf::OnGameplayEffectRemoved);

	if (TriggerEventData != nullptr)
	{
		this->GameplayEffectsContainer->SetCustomDamageCauser((AActor*)TriggerEventData->Instigator);
	}

	// TODO: Allow for effect index specification
	this->GameplayEffectsContainer->AddTargetToEffect(0, Character);
	this->GameplayEffectsContainer->ApplyEffect(0);

	USkeletalMeshComponent* SkeletalMeshComponent = Character->GetMesh();

	if (NiagaraEffectTemplate != nullptr)
	{
		SpawnedNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraEffectTemplate, SkeletalMeshComponent,  FName(), Character->GetActorLocation(), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);;
	}
}

void USGApplyEffectsToSelf::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	
	if (ASCharacterBase* Character = Cast<ASCharacterBase>(GetActorInfo().OwnerActor))
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = Character->GetAbilitySystemComponent())
		{
			AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);
			if (SpawnedNiagaraComponent != nullptr)
			{
				SpawnedNiagaraComponent->DestroyComponent();
			}

			// Remove all active gameplay effects
			TSet<FActiveGameplayEffectHandle> ActiveGameplayEffects;
			this->GameplayEffectsContainer->GetAllActiveGameplayEffects(ActiveGameplayEffects);
			for (auto& ActiveHandle : ActiveGameplayEffects)
			{
				if (!ActiveHandle.IsValid())
				{
					continue;
				}

				AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveHandle);
			}

			this->GameplayEffectsContainer.Reset();
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USGApplyEffectsToSelf::EndAbility(const bool WasCancelled)
{
	Super::EndAbility(WasCancelled);
}


void USGApplyEffectsToSelf::OnGameplayEffectRemoved(const FActiveGameplayEffect& ActiveGameplayEffect)
{
	TSet<FActiveGameplayEffectHandle> ActiveGameplayEffects;
	this->GameplayEffectsContainer->GetAllActiveGameplayEffects(ActiveGameplayEffects);
	if (ActiveGameplayEffects.Contains(ActiveGameplayEffect.Handle))
	{
		this->EndAbility();
	}
}
