// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectActor.generated.h"

class UAbilitySystemComponent;

struct FGameplayEffectsWrapperContainer;


// TODO: Make a component version
UCLASS()
class SOULSGAME_API AGameplayEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameplayEffectActor();

	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable)
	void SetGameplayEffectDataFromAbility(class UMyGameplayAbility* Ability);


	virtual void SetGameplayEffectDataContainer(TWeakPtr<FGameplayEffectsWrapperContainer> Container);
	virtual TWeakPtr<FGameplayEffectsWrapperContainer> GetGameplayEffectsWrapperContainer();

	
protected:
	TWeakPtr<FGameplayEffectsWrapperContainer> GameplayEffectDataContainer = nullptr;

	void SetEffectLevel(int32 EffectIndex, int32 EffectLevel) const;
	void ApplyEffectToActor(int32 EffectIndex, AActor * Other);

	void RemoveGameplayEffects(UAbilitySystemComponent * AbilitySystemComponent);

	bool CanApplyEffectToActor(int32 EffectIndex, AActor* Other) const;
};
