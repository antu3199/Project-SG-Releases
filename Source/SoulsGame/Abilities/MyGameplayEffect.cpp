// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameplayEffect.h"


#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagsManager.h"
#include "MyGameplayAbility.h"
#include "SGAbilitySystemGlobals.h"
#include "SoulsGame/Character/SCharacterBase.h"

// FGameplayEffectsWrapperContainer ========================

FGameplayEffectSpecWrapper::FGameplayEffectSpecWrapper(): Ability(nullptr), Effect(nullptr), ParentContainer(nullptr)
{
}

FGameplayEffectSpecWrapper::~FGameplayEffectSpecWrapper()
{
    GameplayEffectSpecHandle.Clear();
    Ability = nullptr;
    Effect = nullptr;
    
}

FGameplayEffectSpecWrapper::FGameplayEffectSpecWrapper(UMyGameplayAbility * InAbility, UMyGameplayEffect* InEffect,
                                                       FGameplayEffectsWrapperContainer* InParent)
{
    Initialize(InAbility, InEffect, InParent);
}

void FGameplayEffectSpecWrapper::Initialize(UMyGameplayAbility * InAbility, UMyGameplayEffect* InEffect, FGameplayEffectsWrapperContainer* InParent)
{
    Effect = InEffect;
    ParentContainer = InParent;
}

void FGameplayEffectSpecWrapper::ApplyEffectToTargetData()
{
    if (TargetData == nullptr || TargetData.Data.Num() == 0)
    {
        return;
    }
    
    if (TargetData.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Warning: Effect has no targets!"));
        return;
    }

    FGameplayEffectSpecHandle & SpecHandle = GameplayEffectSpecHandle;
    
    if (SpecHandle.IsValid())
    {
        // Activate for each target in target data
        int32 Len = TargetData.Num();

        for (int32 i = 0; i < Len; i++)
        {
            auto TData = TargetData.Get(i);
            if (TData == nullptr)
            {
                UE_LOG(LogTemp, Warning, TEXT("Warning: Handle invalid!"));
                continue;
            }
            
            if (!TData|| !SpecHandle.Data.IsValid())
            {
                UE_LOG(LogTemp, Warning, TEXT("Warning: Handle invalid!"));
                continue;
            }

            if (!SpecHandle.Data->GetContext().IsValid() || !SpecHandle.Data->GetContext().GetInstigatorAbilitySystemComponent())
            {
                UE_LOG(LogTemp, Warning, TEXT("Warning: Handle invalid!"));
                continue;
            }

            TArray<TWeakObjectPtr<AActor>> Actors = TData->GetActors();
            bool bHasStoppingAbility = false;

            for (const TWeakObjectPtr<AActor>& Actor : Actors)
            {
                const UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor.Get());
                if (!AbilitySystem)
                {
                    continue;
                }
                
                if (AbilitySystem->HasMatchingGameplayTag(USGAbilitySystemGlobals::GetSG().Tag_State_Countering))
                {
                    bHasStoppingAbility = true;
                    FGameplayEventData EventPayload;
                    EventPayload.EventTag = USGAbilitySystemGlobals::GetSG().Tag_State_Countering;
                    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor.Get(), USGAbilitySystemGlobals::GetSG().Tag_Triggers_Ability_Activate, EventPayload);
                    break;
                }
            }

            if (bHasStoppingAbility)
            {
                continue;
            }

            //SpecHandle.Data->SetLevel(2);
            float CurLevel = SpecHandle.Data->GetLevel();
            
            TArray<FActiveGameplayEffectHandle> Handles = TData->ApplyGameplayEffectSpec(*SpecHandle.Data.Get());
            UE_LOG(LogTemp, Warning, TEXT("Applied effect at level: %f"), CurLevel);
            //TArray<FActiveGameplayEffectHandle> Handles = ApplyGameplayEffectSpec(TData, *SpecHandle.Data.Get(), FPredictionKey());
            this->ActiveGameplayEffectHandles.Append(Handles);
        }

        TargetData.Clear();
    }
}

/*
TArray<FActiveGameplayEffectHandle> FGameplayEffectSpecWrapper::ApplyGameplayEffectSpec(FGameplayAbilityTargetData* TData, FGameplayEffectSpec& InSpec, FPredictionKey PredictionKey)
{
    TArray<FActiveGameplayEffectHandle>	AppliedHandles;

    if (!ensure(InSpec.GetContext().IsValid() && InSpec.GetContext().GetInstigatorAbilitySystemComponent()))
    {
        return AppliedHandles;
    }

    TArray<TWeakObjectPtr<AActor> > Actors = TData->GetActors();
	
    AppliedHandles.Reserve(Actors.Num());

    for (TWeakObjectPtr<AActor>& TargetActor : Actors)
    {
        UAbilitySystemComponent* TargetComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor.Get());

        if (TargetComponent)
        {
            // We have to make a new effect spec and context here, because otherwise the targeting info gets accumulated and things take damage multiple times
            FGameplayEffectSpec	SpecToApply(InSpec);
            FGameplayEffectContextHandle EffectContext = SpecToApply.GetContext().Duplicate();
            SpecToApply.SetContext(EffectContext);

            TData->AddTargetDataToContext(EffectContext, false);

            UAbilitySystemComponent* InstigatorAbilityComponent = EffectContext.GetInstigatorAbilitySystemComponent();
            AppliedHandles.Add(TargetComponent->ApplyGameplayEffectSpecToSelf(SpecToApply, PredictionKey));
            
            //AppliedHandles.Add(EffectContext.GetInstigatorAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(SpecToApply, TargetComponent, PredictionKey));
        }
    }

    return AppliedHandles;
}
*/

void FGameplayEffectSpecWrapper::AddTarget(AActor* TargetActor, bool bCheckEffectOverlapBehaviour)
{
    if (TargetActor == nullptr)
    {
        return;
    }

    if (bCheckEffectOverlapBehaviour)
    {
        if (!ValidOverlapTarget(TargetActor))
        {
            return;
        }
    }

    FGameplayAbilityTargetData_ActorArray * NewData = new FGameplayAbilityTargetData_ActorArray();
    NewData->TargetActorArray.Add(MakeWeakObjectPtr(TargetActor));
    TargetData.Add(NewData);
}

void FGameplayEffectSpecWrapper::ClearTargets()
{
    TargetData.Clear();
}

bool FGameplayEffectSpecWrapper::ValidOverlapTarget(AActor* TargetActor) const
{
    if (ParentContainer == nullptr)
    {
        return false;
    }

    FSGOverlapContext Context(ParentContainer->InstigatorActor.Get(), TargetActor, Effect->OverlapBehaviour);
    return Context.ShouldOverlap();
}

// FGameplayEffectSpecWrapper END

void FGameplayEffectsWrapperContainer::Initialize(AActor* InInstigatorActor)
{
    InstigatorActor = MakeWeakObjectPtr(InInstigatorActor);
}

// FGameplayEffectWrapperContainer
FGameplayEffectSpecWrapper& FGameplayEffectsWrapperContainer::AddEffect(UMyGameplayAbility * InAbility, UMyGameplayEffect* InEffect, float Duration, int Level)
{
    FGameplayEffectSpecWrapper Wrapper(InAbility, InEffect);
    Wrapper.GameplayEffectSpecHandle = InAbility->MakeOutgoingGameplayEffectSpec(InEffect->GetClass(), Level);
    check(Wrapper.GameplayEffectSpecHandle.IsValid());
    if (Duration != -1)
    {
        Wrapper.GameplayEffectSpecHandle.Data.Get()->SetDuration(Duration, false);
    }

    Wrapper.ParentContainer = this;

    float CurLevel = Wrapper.GameplayEffectSpecHandle.Data->GetLevel();
    UE_LOG(LogTemp, Warning, TEXT("AddEffect: CurLevel: %f"), CurLevel);
    
    Effects.Add(Wrapper);
    return Effects.Last();
}

bool FGameplayEffectsWrapperContainer::HasEffects() const
{
    return Effects.Num() > 0;
}

void FGameplayEffectsWrapperContainer::Reset()
{
    Effects.Reset();
}

void FGameplayEffectsWrapperContainer::ApplyEffect(int32 EffectIndex)
{
    if (!Effects.IsValidIndex(EffectIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid effect index: %d"), EffectIndex);
        return;
    }

    FGameplayEffectSpecWrapper& Effect = Effects[EffectIndex];
    if (Effect.Effect == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Effect was null!"));
        return;
    }

    OnBeforeApplyEffectDelegate.ExecuteIfBound(Effect, Effect.CachedTargets);

    Effect.ApplyEffectToTargetData();

    OnAfterApplyEffectDelegate.ExecuteIfBound(Effect, Effect.CachedTargets);
}

void FGameplayEffectsWrapperContainer::AddTargetToEffect(int32 EffectIndex, AActor* TargetActor)
{
    if (!Effects.IsValidIndex(EffectIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid effect index: %d"), EffectIndex);
        return;
    }

    FGameplayEffectSpecWrapper& Effect = Effects[EffectIndex];
    if (!CanApplyEffectToActor(EffectIndex, TargetActor))
    {
        return;
    }

    Effect.AddTarget(TargetActor);
}

bool FGameplayEffectsWrapperContainer::CanApplyEffectToActor(int32 EffectIndex, AActor * TargetActor)
{
    if (!Effects.IsValidIndex(EffectIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid effect index: %d"), EffectIndex);
        return false;
    }
    
    if (TargetActor == nullptr)
    {
        return true;
    }

    FGameplayEffectSpecWrapper& Effect = Effects[EffectIndex];
    const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
    if (ASC == nullptr)
    {
        return false;
    }

    if (Effect.Effect == nullptr)
    {
        return false;
    }

    FGameplayTagContainer OwnedGameplayTags;
    ASC->GetOwnedGameplayTags(OwnedGameplayTags);

    if (!Effect.Effect->GrantedApplicationImmunityTags.RequirementsMet(OwnedGameplayTags))
    {
        return false;
    }

    FSGOverlapContext OverlapContext(InstigatorActor.Get(), TargetActor, Effect.Effect->OverlapBehaviour);
    return OverlapContext.ShouldOverlap();
}

int32 FGameplayEffectsWrapperContainer::GetEffectIndexForTag(FGameplayTag Tag) const
{
    for (int32 i = 0; i < Effects.Num(); i++)
    {
        const FGameplayEffectSpecWrapper& Effect = Effects[i];
        if (Effect.Effect == nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("Effect was null!"));
            continue;
        }

        if (Effect.Effect->HasMatchingGameplayTag(Tag))
        {
            return i;
        }
    }

    return -1;
}

void FGameplayEffectsWrapperContainer::ResetTargets()
{
    for (auto & Effect : Effects)
    {
        if (Effect.Effect == nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("Effect was null!"));
            continue;
        }

        Effect.ClearTargets();
    }
}

void FGameplayEffectsWrapperContainer::SetEffectLevel(int32 EffectIndex, int32 Level)
{
    if (!Effects.IsValidIndex(EffectIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid effect index: %d"), EffectIndex);
        return;
    }

    FGameplayEffectSpecWrapper& Effect = Effects[EffectIndex];
    Effect.GameplayEffectSpecHandle.Data->SetLevel(Level);

    float CurLevel = Effect.GameplayEffectSpecHandle.Data->GetLevel();
    UE_LOG(LogTemp, Warning, TEXT("Set effect level: %d. CurLevel: %f"), Level, CurLevel);
}

void FGameplayEffectsWrapperContainer::GetAllActiveGameplayEffects(TSet<FActiveGameplayEffectHandle>& OutEffects) const
{
    for (auto& Effect : Effects)
    {
        OutEffects.Append(Effect.ActiveGameplayEffectHandles);
    }
}

void FGameplayEffectsWrapperContainer::RemoveActiveGameplayEffectHandle(FActiveGameplayEffectHandle Handle)
{
    for (auto& Effect : Effects)
    {
        if (Effect.ActiveGameplayEffectHandles.Contains(Handle))
        {
            Effect.ActiveGameplayEffectHandles.Remove(Handle);
        }
    }
}

void FGameplayEffectsWrapperContainer::SetCustomDamageCauser(AActor* Causer)
{
    for (auto& Effect : Effects)
    {
        if (!Effect.GameplayEffectSpecHandle.IsValid() || !Effect.GameplayEffectSpecHandle.Data.IsValid())
        {
            continue;
        }


        Effect.GameplayEffectSpecHandle.Data->GetContext().Get()->SetEffectCauser(Causer);
    }
}

// FGameplayEffectWrapperContainer END

// UMyGameplayEffect =======================
void UMyGameplayEffect::GetTargets_UseEventData(ASCharacterBase* TargetingCharacter, AActor* TargetingActor,
                                                const FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors)
{
    const FHitResult * FoundHitResult = EventData.ContextHandle.GetHitResult();
    if (FoundHitResult)
    {
        OutHitResults.Add(*FoundHitResult);
    }
    else if (EventData.Target)
    {
        OutActors.Add(const_cast<AActor*>(EventData.Target));
    }

}
