// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayEffectActor.h"


#include "AbilitySystemComponent.h"
#include "MyGameplayAbility.h"

// Sets default values
AGameplayEffectActor::AGameplayEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AGameplayEffectActor::BeginDestroy()
{
	Super::BeginDestroy();

	GameplayEffectDataContainer = nullptr;
}

void AGameplayEffectActor::SetGameplayEffectDataFromAbility(UMyGameplayAbility* Ability)
{
	if (Ability)
	{
		GameplayEffectDataContainer = Ability->GameplayEffectsContainer;
	}
}

void AGameplayEffectActor::SetGameplayEffectDataContainer(TWeakPtr<FGameplayEffectsWrapperContainer> Container)
{
	GameplayEffectDataContainer = Container;
}

TWeakPtr<FGameplayEffectsWrapperContainer> AGameplayEffectActor::GetGameplayEffectsWrapperContainer()
{
	return GameplayEffectDataContainer;
}

void AGameplayEffectActor::SetEffectLevel(int32 EffectIndex, int32 EffectLevel) const
{
	if (GameplayEffectDataContainer == nullptr)
	{
		return;
	}

	GameplayEffectDataContainer.Pin()->SetEffectLevel(EffectIndex, EffectLevel);
}

void AGameplayEffectActor::ApplyEffectToActor(int32 EffectIndex, AActor* Other)
{
	if (GameplayEffectDataContainer == nullptr)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Apply effect to target: %s"), *Other->GetActorNameOrLabel());

	GameplayEffectDataContainer.Pin()->AddTargetToEffect(EffectIndex, Other);
	GameplayEffectDataContainer.Pin()->ApplyEffect(EffectIndex);
}

void AGameplayEffectActor::RemoveGameplayEffects(UAbilitySystemComponent * AbilitySystemComponent)
{
	if (!AbilitySystemComponent || !GameplayEffectDataContainer.IsValid())
	{
		return;
	}
	
	for (auto EffectSpec : GameplayEffectDataContainer.Pin()->Effects)
	{
		for (auto &ActiveHandle : EffectSpec.ActiveGameplayEffectHandles)
		{
			AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveHandle);
		}
	}
}

bool AGameplayEffectActor::CanApplyEffectToActor(int32 EffectIndex, AActor* Other) const
{
	if (GameplayEffectDataContainer == nullptr)
	{
		return false;
	}

	return GameplayEffectDataContainer.Pin()->CanApplyEffectToActor(EffectIndex, Other);
}




