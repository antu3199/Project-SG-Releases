// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityConfectioniteBase.h"


#include "AbilityTask_WaitDelayWithEvent.h"
#include "SoulsGame/Character/ConfectioniteCharacter.h"


void UAbilityConfectioniteBase::PreActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::PreActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	TaskShouldEndAbility = false;
	AbilityTask = nullptr;

	AConfectioniteCharacter * Character = Cast<AConfectioniteCharacter>(GetActorInfo().OwnerActor);
	if (Character == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR: No character!"));
		return;
	}

	ConfectioniteCharacter = Character;

	CharacterBase = ConfectioniteCharacter->GetAttachedCharacter();
}

bool UAbilityConfectioniteBase::HandleGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (Super::HandleGameplayEvent(EventTag, Payload))
	{
		return true;
	}
	
	FString Tag = EventTag.ToString();

	bool bHandled = false;

	if (Tag == "Triggers.Confectionite.EnableWeaponHitbox")
	{
		EnableWeaponHitbox(true);
		bHandled = true;
	}
	else if (Tag == "Triggers.Confectionite.DisableWeaponHitbox")
	{
		EnableWeaponHitbox(false);
		bHandled = true;
	}
	else if (Tag == "Triggers.Confectionite.ApplyEffectsToProjectiles")
	{
		//ConfectioniteCharacter->SetProjectileGameplayEffect(GameplayEffectsContainer);
	}
	else if (Tag == "Triggers.Confectionite.ReattachToCharacter")
	{
		EndOnMontageCompleted = false;
		ConfectioniteCharacter->DeactivatePS();

		FGameplayTagContainer EmptyContainer;

		AbilityTask = UAbilityTask_WaitDelayWithEvent::WaitDelayWithEvent(this, Payload->EventMagnitude, EmptyContainer);
		AbilityTask->OnFinish.AddDynamic(this, &UAbilityConfectioniteBase::ReattachConfectioniteAndEndAbility);
		AbilityTask->ReadyForActivation();
		bHandled = true;
	}
	else if (Tag == "Ability.Donut.DonutBlast")
	{
		// EndOnMontageCompleted = false;
		// ConfectioniteCharacter->DeactivatePS();
		// ConfectioniteCharacter->SetProjectileGameplayEffect(GameplayEffectsContainer); // May be moved to somewhere else in the future.
		// 
		// ConfectioniteCharacter->DoDonutBlast([=] ()
		// {
		// 	this->EndAbility();
		// });
		bHandled = true;
	}
	else if (Tag == "Ability.Donut.DonutSpiral")
	{
		//EndOnMontageCompleted = false;
		//ConfectioniteCharacter->DeactivatePS();
		//ConfectioniteCharacter->SetProjectileGameplayEffect(GameplayEffectsContainer); // May be moved to somewhere else in the future.
		//
		//ConfectioniteCharacter->DoDonutSpiral([=] ()
		//{
		//	this->EndAbility();
		//});
		bHandled = true;
	}

	return bHandled;
}

void UAbilityConfectioniteBase::OnCompleted(FGameplayTag GameplayTag, FGameplayEventData GameplayEventData)
{
	if (AbilityTask == nullptr || !AbilityTask->IsActive())
	{
		Super::OnCompleted(GameplayTag, GameplayEventData);
	}
	else
	{
		TaskShouldEndAbility = true;
	}
}

void UAbilityConfectioniteBase::ReattachConfectioniteAndEndAbility()
{
	ConfectioniteCharacter->ReattachToCharacter();

	if (TaskShouldEndAbility)
	{
		this->EndAbility();
	}
}

void UAbilityConfectioniteBase::EnableWeaponHitbox(bool Enabled)
{
	AAbilityActor * AbilityActor = ConfectioniteCharacter->GetHitboxActor();
	AbilityActor->SetGameplayEffectDataContainer(GameplayEffectsContainer);
	AbilityActor->EnableCollision(Enabled);
}
