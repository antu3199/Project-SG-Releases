#include "NS_SetEffectLevel.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "SendGameplayTagEventNS.h"
#include "SoulsGame/Abilities/SGAbilitySystemGlobals.h"

void UNS_SetEffectLevel::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	AActor * Actor = MeshComp->GetOwner();
	UAbilitySystemComponent* Component = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);

	if (Component != nullptr)
	{
		FGameplayEventData EventPayload;
		USGSetEffectLevel_GameplayEventDataObject* Object = NewObject<USGSetEffectLevel_GameplayEventDataObject>();
		Object->EffectIndex = LevelEffectIndex;
		Object->Level = LevelEffectLevel;
		
		EventPayload.OptionalObject = Object;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, USGAbilitySystemGlobals::GetSG().Tag_Triggers_DataObject, EventPayload);
	}
}

void UNS_SetEffectLevel::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);
}
