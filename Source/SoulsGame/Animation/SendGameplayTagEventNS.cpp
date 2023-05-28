// Fill out your copyright notice in the Description page of Project Settings.


#include "SendGameplayTagEventNS.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"

void USendGameplayTagEventNS::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                            float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	if (MeshComp == nullptr)
	{
		return;
	}

	if (MeshComp->GetOwner() == nullptr)
	{
	    return;
	}

	if (!GameplayTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR: Gameplay tag not valid"));
		return;
	}


	AActor * Actor = MeshComp->GetOwner();
	UAbilitySystemComponent* Component = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);

	if (Component != nullptr)
	{
		if (bSetLevel)
		{
			EventPayload.OptionalObject = ConstructSetEffectLevel();
		}

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, GameplayTag, EventPayload);
	}

}

void USendGameplayTagEventNS::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	if (MeshComp == nullptr)
	{
		return;
	}

	if (MeshComp->GetOwner() == nullptr)
	{
		return;
	}

	if (!OnEndGameplayTag.IsValid())
	{
		// UE_LOG(LogTemp, Warning, TEXT("ERROR: Gameplay tag not valid"));
		return;
	}

	AActor * Actor = MeshComp->GetOwner();
	UAbilitySystemComponent* Component = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);

	if (Component != nullptr)
	{
		if (bSetLevel)
		{
			EventPayload.OptionalObject = ConstructSetEffectLevel();
		}
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, OnEndGameplayTag, EventEndPayload);
	}
}

FString USendGameplayTagEventNS::GetNotifyName_Implementation() const
{
	if (GameplayTag.IsValid())
	{
		return "NS: " + GameplayTag.ToString();
	}

	if (OnEndGameplayTag.IsValid())
	{
		return "NS: " + OnEndGameplayTag.ToString();
	}
	
	return Super::GetNotifyName_Implementation();
}

USGSetEffectLevel_GameplayEventDataObject* USendGameplayTagEventNS::ConstructSetEffectLevel() const
{
	USGSetEffectLevel_GameplayEventDataObject* Object = NewObject<USGSetEffectLevel_GameplayEventDataObject>();
	Object->EffectIndex = SetLevelEffectIndex;
	Object->Level = SetLevelEffectLevel;
	return Object;
}
