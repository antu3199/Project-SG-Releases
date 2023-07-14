// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoulsGame/SGAbilitySystem/SGEffect.h"
#include "SGEffectActor.generated.h"

class UAbilitySystemComponent;

struct FGameplayEffectsWrapperContainer;


// TODO: Make a component version
UCLASS()
class SOULSGAME_API ASGEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASGEffectActor();

	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable)
	virtual void SetEffectContext(FSGEffectInstigatorContext InEffectContext);
	
protected:
	FSGEffectInstigatorContext EffectContext;
};
