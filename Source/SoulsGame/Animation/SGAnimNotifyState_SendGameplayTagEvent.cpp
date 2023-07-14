// Fill out your copyright notice in the Description page of Project Settings.


#include "SGAnimNotifyState_SendGameplayTagEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "SoulsGame/SGAbilitySystem/SGAbilityBlueprintLibrary.h"
#include "SoulsGame/SGAbilitySystem/SGAbilityComponent.h"

void USGAnimNotifyState_SendGameplayTagEvent::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
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
	
	if (USGAbilityComponent* SGComponent = Actor->FindComponentByClass<USGAbilityComponent>())
	{
		if (bSetLevel)
		{
			EventPayload.OptionalObject = ConstructSetEffectLevel();
		}

		USGAbilitySystemBlueprintLibrary::SendAbilityEventToActor(Actor, GameplayTag, EventPayload);
	}

}

void USGAnimNotifyState_SendGameplayTagEvent::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
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

	if (USGAbilityComponent* SGComponent = Actor->FindComponentByClass<USGAbilityComponent>())
	{
		if (bSetLevel)
		{
			EventEndPayload.OptionalObject = ConstructSetEffectLevel();
		}

		USGAbilitySystemBlueprintLibrary::SendAbilityEventToActor(Actor, OnEndGameplayTag, EventEndPayload);
	}
}

FString USGAnimNotifyState_SendGameplayTagEvent::GetNotifyName_Implementation() const
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

USGSetEffectLevel_GameplayEventDataObject* USGAnimNotifyState_SendGameplayTagEvent::ConstructSetEffectLevel() const
{
	USGSetEffectLevel_GameplayEventDataObject* Object = NewObject<USGSetEffectLevel_GameplayEventDataObject>();
	Object->EffectIndex = SetLevelEffectIndex;
	Object->Level = SetLevelEffectLevel;
	return Object;
}
