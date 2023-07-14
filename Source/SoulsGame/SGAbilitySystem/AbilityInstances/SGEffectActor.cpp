// Fill out your copyright notice in the Description page of Project Settings.


#include "SGEffectActor.h"


#include "AbilitySystemComponent.h"

// Sets default values
ASGEffectActor::ASGEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASGEffectActor::BeginDestroy()
{
	Super::BeginDestroy();
}

void ASGEffectActor::SetEffectContext(
	FSGEffectInstigatorContext InEffectContext)
{
	EffectContext = InEffectContext;
}


