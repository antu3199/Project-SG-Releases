#include "SAnimNotifyState_CreateHitbox.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagsManager.h"
#include "HitboxGameplayEventDataObject.h"
#include "SoulsGame/SBlueprintLibrary.h"
#include "SoulsGame/SHitboxManager.h"
#include "SoulsGame/SUtils.h"
#include "SoulsGame/Abilities/SGAbilitySystemGlobals.h"

void USAnimNotifyState_CreateHitbox::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                   float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	UWorld* World = MeshComp->GetWorld();

	if (bOnlyActivateInPreview && World->HasBegunPlay())
	{
		return;
	}

	USHitboxManager* HitboxManager = SBlueprintLibrary::GetHitboxManager();
	if (HitboxManager == nullptr)
	{
		return;
	}

	if (HitboxId != -1)
	{
		HitboxManager->RemoveHitbox(HitboxId);
		HitboxId = -1;
	}

	HitboxId = CreateHitbox(MeshComp, HitboxManager);
	//HitboxId = HitboxManager->CreateBox(MeshComp, SocketName.ToString(), BoxExtents, AttachmentRule, TranslationOffset, RotationOffset);
	HitboxManager->SetCollisionProfile(HitboxId, CollisionParams);

	
	AActor * Actor = MeshComp->GetOwner();
	UAbilitySystemComponent* Component = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);

	if (Component != nullptr)
	{
		FGameplayTag SetEffectTag = USGAbilitySystemGlobals::GetSG().Tag_Triggers_Character_SetHitboxEffect;
		FGameplayEventData EventData;
		// TODO: Check if this gets GCed? 
		USGCreateHitbox_GameplayEventDataObject* Object1 = NewObject<USGCreateHitbox_GameplayEventDataObject>();
		Object1->HitboxId = HitboxId;
		Object1->OverlapInterval = HitboxOverlapInterval;
		EventData.EventTag = EventTag;

		EventData.OptionalObject = Object1;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, SetEffectTag, EventData);
	}
}

void USAnimNotifyState_CreateHitbox::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	if (bOnlyActivateInPreview && !GIsPlayInEditorWorld)
	{
		return;
	}
	
	USHitboxManager* HitboxManager = SBlueprintLibrary::GetHitboxManager();
	if (HitboxManager == nullptr)
	{
		return;
	}

	if (HitboxId != -1)
	{
		HitboxManager->RemoveHitbox(HitboxId);
		HitboxId = -1;
	}
}

int32 USAnimNotifyState_CreateHitbox::CreateHitbox(USkeletalMeshComponent* MeshComp, USHitboxManager* HitboxManager)
{
	UE_LOG(LogTemp, Error, TEXT("Pure virutal!"));
	return -1;
}


