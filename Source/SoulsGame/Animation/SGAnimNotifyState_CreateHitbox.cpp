#include "SGAnimNotifyState_CreateHitbox.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagsManager.h"
#include "SoulsGame/SGUtils.h"
#include "SoulsGame/SGAbilitySystem/SGAbilityBlueprintLibrary.h"

void USGAnimNotifyState_CreateHitbox::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                   float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	UWorld* World = MeshComp->GetWorld();

	if (bOnlyActivateInPreview && World->HasBegunPlay())
	{
		return;
	}
	AActor * Actor = MeshComp->GetOwner();

	// TODO: Add visualizer outside of game
	FSGHitboxParams RealParams = HitboxParams;
	RealParams.EffectContext.OwnerActor = MakeWeakObjectPtr(Actor);
	RealParams.EffectContext.AvatarActor = MakeWeakObjectPtr(Actor);

	if (USGHitboxSubsystem* HitboxSubsystem = FSGUtils::GetGameInstanceSubsystem<USGHitboxSubsystem>(MeshComp->GetWorld()))
	{
		HitboxId = CreateHitbox(MeshComp, RealParams);

		ensure(HitboxId != INDEX_NONE);

		USGHitboxObject* Hitbox = HitboxSubsystem->GetHitbox(HitboxId);
		ensure(IsValid(Hitbox));

		static const FGameplayTag TriggerTag =  UGameplayTagsManager::Get().RequestGameplayTag(FName("Triggers.Shared.CreateHitbox"));
		FSGAbilityEventData HitboxPayload;
		HitboxPayload.EventTag = TriggerTag;
		HitboxPayload.OptionalObject = Hitbox;
		HitboxPayload.OptionalInt1 = HitboxId;
	
		USGAbilitySystemBlueprintLibrary::SendAbilityEventToActor(Actor, TriggerTag, HitboxPayload);
	}
}

void USGAnimNotifyState_CreateHitbox::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	if (bOnlyActivateInPreview && !GIsPlayInEditorWorld)
	{
		return;
	}

	if (HitboxId != -1)
	{
		if (USGHitboxSubsystem* HitboxSubsystem = FSGUtils::GetGameInstanceSubsystem<USGHitboxSubsystem>(MeshComp->GetWorld()))
		{
			HitboxSubsystem->RemoveHitbox(HitboxId);
		}
		HitboxId = -1;
	}
}

int32 USGAnimNotifyState_CreateHitbox::CreateHitbox(USkeletalMeshComponent* MeshComp,
	const FSGHitboxParams& Params)
{
	UE_LOG(LogTemp, Error, TEXT("Pure virutal!"));
	return -1;
}



