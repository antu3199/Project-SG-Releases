// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayMontageAndWaitTask.h"


#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "CharacterAbilitySystemComponent.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/Character/SHumanoidPlayerCharacter.h"

UPlayMontageAndWaitTask::UPlayMontageAndWaitTask(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->TaskData.Rate = 1.f;
    this->TaskData.StopWhenAbilityEnds = true;
}

void UPlayMontageAndWaitTask::Activate()
{
    Super::Activate();

    if (UAbilityTask::Ability != nullptr && UAbilityTask::Ability != this->TaskData.OwningAbility)
    {
        UE_LOG(LogTemp, Warning, TEXT("Warning: Task ability not the same as owning ability!"));
        return;
    }
    UCharacterAbilitySystemComponent* MyAbilitySystemComponent = this->GetTargetAbilitySystemComponent();
    
    if (!MyAbilitySystemComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("Error: Could not find ability system!"));
        return;
    }

    const FGameplayAbilityActorInfo *ActorInfo = this->Ability->GetCurrentActorInfo();
    UAnimInstance * AnimInstance = ActorInfo->GetAnimInstance();
    if (AnimInstance == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Error: Could not find anim instance!"));
        return;
    }

    if (!ShouldBroadcastAbilityTaskDelegates())
    {
        return;
    }

    const float CanPlayMontage = MyAbilitySystemComponent->PlayMontage(this->Ability, this->Ability->GetCurrentActivationInfo(), TaskData.MontageToPlay, this->TaskData.Rate, this->TaskData.StartSection);
    if (!CanPlayMontage)
    {
        UE_LOG(LogTemp, Error, TEXT("Error: Failed to play Montage!"));
        return;
    }

    CancelledDelegateHandle = this->Ability->OnGameplayAbilityCancelled.AddUObject(this, &UPlayMontageAndWaitTask::OnAbilityCancelled);

    this->BlendingOutDelegate.BindUObject(this, &UPlayMontageAndWaitTask::OnMontageBlendingOut);
    AnimInstance->Montage_SetBlendingOutDelegate(this->BlendingOutDelegate, TaskData.MontageToPlay);

    this->MontageEndedDelegate.BindUObject(this, &UPlayMontageAndWaitTask::OnMontageEnded);
    AnimInstance->Montage_SetEndDelegate(this->MontageEndedDelegate, TaskData.MontageToPlay);

    // Set Anim translation scale
    ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
    if (Character && (Character->GetLocalRole() == ROLE_Authority ||
        (Character->GetLocalRole() == ROLE_AutonomousProxy && this->Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
    {
        Character->SetAnimRootMotionTranslationScale(this->TaskData.AnimRootMotionTranslationScale);
    }

    this->PlayedMontage = true;

    if (!this->PlayedMontage && ShouldBroadcastAbilityTaskDelegates())
    {
        OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
    }
    
}

void UPlayMontageAndWaitTask::ExternalCancel()
{
    Super::ExternalCancel();
}

void UPlayMontageAndWaitTask::OnDestroy(bool AbilityEnded)
{

    if (Ability)
    {
        Ability->OnGameplayAbilityCancelled.Remove(CancelledDelegateHandle);
        if (AbilityEnded && TaskData.StopWhenAbilityEnds)
        {
            // Not sure if need this...
            // Messes up if we have multiple executions
            // this->StopPlayingMontage();
        }
    }

    ASHumanoidPlayerCharacter * Character = Cast<ASHumanoidPlayerCharacter>(GetAvatarActor());
    if (Character)
    {
        // if (Character->GetMontageCancellable() && Character->GetQueuedAction() != nullptr)
        // {
        //     UE_LOG(LogTemp, Warning, TEXT("WARNING: Maybe decrease blend time?"));
        // }
    }

    Super::OnDestroy(AbilityEnded);
}

bool UPlayMontageAndWaitTask::StopPlayingMontage()
{
    const FGameplayAbilityActorInfo *ActorInfo = this->Ability->GetCurrentActorInfo();

    if (!ActorInfo)
    {
        return false;
    }

    UAnimInstance * AnimInstance = ActorInfo->GetAnimInstance();
    if (AnimInstance == nullptr)
    {
        return false;
    }

    if (!this->AbilitySystemComponent || !this->Ability)
    {
        return false;
    }

    UE_LOG(LogTemp, Warning, TEXT("Stop playing montage"));

    
    FAnimMontageInstance * MontageInstance = AnimInstance->GetActiveInstanceForMontage(TaskData.MontageToPlay);
    if (MontageInstance)
    {
        MontageInstance->OnMontageBlendingOutStarted.Unbind();
        MontageInstance->OnMontageEnded.Unbind();
    }

    ASCharacterBase * Character = Cast<ASCharacterBase>(GetAvatarActor());
    if (Character && !Character->GetIsDead())
    {
        Character->SetStopRootMotionOnHit(false);
        this->AbilitySystemComponent->CurrentMontageStop();
    }

    
    return true;
}

UCharacterAbilitySystemComponent* UPlayMontageAndWaitTask::GetTargetAbilitySystemComponent() const
{
    return Cast<UCharacterAbilitySystemComponent>(this->AbilitySystemComponent);
}

void UPlayMontageAndWaitTask::OnAbilityCancelled()
{
    if (StopPlayingMontage())
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            this->OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());

        }
    }
}

void UPlayMontageAndWaitTask::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (!bInterrupted)
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            this->OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
        }
    }
}

void UPlayMontageAndWaitTask::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
    // Hack to ignore Blending ot if null meta data
    bool HackyMetaData = false;
    for (auto MetaData : Montage->GetMetaData())
    {
        if (MetaData == nullptr)
        {
            HackyMetaData = true;
        }
    }

    if (HackyMetaData)
    {
        Montage->RemoveMetaData(nullptr);
        return;
    }


    /*
    if (bInterrupted)
    {
        return;
    }
    Montage->GetMetaData();
    Montage->RemoveMetaData(nullptr);

    Montage->AddMetaData(nullptr);
    */
    
   
    // Clear Animating aiblity, and set root motion state
    if (this->Ability && this->Ability->GetCurrentMontage() == TaskData.MontageToPlay)
    {
        this->AbilitySystemComponent->ClearAnimatingAbility(this->Ability);
        ACharacter * Character = Cast<ACharacter>(GetAvatarActor());
        if (Character && (Character->GetLocalRole() == ROLE_Authority
            || (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted))
         )
        {
            //Character->SetAnimRootMotionTranslationScale(1.f);
        }
    }

    if (!ShouldBroadcastAbilityTaskDelegates())
    {
        return;
    }

    if (bInterrupted)
    {
        this->OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
    }
    else
    {
        this->OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
    }
}

UPlayMontageAndWaitTask * UPlayMontageAndWaitTask::CreatePlayMontageAndWaitEvent(FPlayMontageAndWaitTaskData& TaskData)
{
    UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(TaskData.Rate);
    
    UPlayMontageAndWaitTask * MyObj = NewAbilityTask<UPlayMontageAndWaitTask>(TaskData.OwningAbility, TaskData.TaskInstanceName);
    MyObj->TaskData = TaskData; // Copy data by value for ownership
    
    return MyObj;
}
