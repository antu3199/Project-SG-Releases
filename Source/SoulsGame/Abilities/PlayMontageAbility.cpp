// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayMontageAbility.h"

#include "PlayMontageAndWaitTask.h"
#include "Abilities/Tasks/AbilityTask.h"

#include "GameplayTagsManager.h"
#include "SGAbilitySystemGlobals.h"
#include "SoulsGame/Character/SCharacterBase.h"

bool UPlayMontageAbility::GetEndOnMontageCompleted() const
{
    return EndOnMontageCompleted;
}

UPlayMontageAndWaitTask* UPlayMontageAbility::GetPlayMontageTask() const
{
    return PlayMontageTask;
}

bool UPlayMontageAbility::CommitAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    FGameplayTagContainer* OptionalRelevantTags)
{
    return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void UPlayMontageAbility::PreActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::PreActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    if (PlayMontage)
    {
        if (this->PlayMontageTaskData.OwningAbility == nullptr)
        {
            this->PlayMontageTaskData.OwningAbility = this;
        }

        if (UAnimMontage* Montage = OverrideMontage())
        {
            this->PlayMontageTaskData.MontageToPlay = Montage;
        }
        
        PlayMontageTask = UPlayMontageAndWaitTask::CreatePlayMontageAndWaitEvent(this->PlayMontageTaskData);
        
        PlayMontageTask->OnCompleted.AddDynamic(this, &UPlayMontageAbility::OnCompleted);
        PlayMontageTask->OnBlendOut.AddDynamic(this, &UPlayMontageAbility::OnBlendOut);
        PlayMontageTask->OnInterrupted.AddDynamic(this, &UPlayMontageAbility::OnInterrupted);
        PlayMontageTask->OnCancelled.AddDynamic(this, &UPlayMontageAbility::OnCancelled);
        
        PlayMontageTask->ReadyForActivation();
    }
}

void UPlayMontageAbility::OnBlendOut(const FGameplayTag GameplayTag, FGameplayEventData GameplayEventData)
{
    BroadcastAbilityEvent(USGAbilitySystemGlobals::GetSG().Tag_AbilityEvent_AbilityBroadcasts_MontageEnd, GameplayEventData);
    
    //this->EndAbility(this->CurrentSpecHandle, this->CurrentActorInfo, this->CurrentActivationInfo, true, false);
}

void UPlayMontageAbility::OnInterrupted(const FGameplayTag GameplayTag, FGameplayEventData GameplayEventData)
{
    BroadcastAbilityEvent(USGAbilitySystemGlobals::GetSG().Tag_AbilityEvent_AbilityBroadcasts_MontageEnd, GameplayEventData);
    
    
    this->EndAbility(this->CurrentSpecHandle, this->CurrentActorInfo, this->CurrentActivationInfo, true, true);
}

void UPlayMontageAbility::OnCancelled(const FGameplayTag GameplayTag, FGameplayEventData GameplayEventData)
{
    BroadcastAbilityEvent(USGAbilitySystemGlobals::GetSG().Tag_AbilityEvent_AbilityBroadcasts_MontageEnd, GameplayEventData);
    
    
    this->EndAbility(this->CurrentSpecHandle, this->CurrentActorInfo, this->CurrentActivationInfo, true, true);
}

void UPlayMontageAbility::OnCompleted(const FGameplayTag GameplayTag, FGameplayEventData GameplayEventData)
{
    BroadcastAbilityEvent(USGAbilitySystemGlobals::GetSG().Tag_AbilityEvent_AbilityBroadcasts_MontageEnd, GameplayEventData);
    
    OnMontageFinished.Broadcast(this);
    
    if (EndOnMontageCompleted)
    {
        this->EndAbility(this->CurrentSpecHandle, this->CurrentActorInfo, this->CurrentActivationInfo, true, false);
    }
}

bool UPlayMontageAbility::HandleGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData* Payload)
{
   if (Super::HandleGameplayEvent(EventTag, Payload))
   {
       return true;
   }
    
    return false;
}

UAnimMontage* UPlayMontageAbility::OverrideMontage_Implementation()
{
    return nullptr;
}


void UPlayMontageAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPlayMontageAbility::EndAbility(const bool WasCancelled)
{
    Super::EndAbility(WasCancelled);
}





