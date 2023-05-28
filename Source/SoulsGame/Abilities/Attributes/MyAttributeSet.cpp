// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAttributeSet.h"


#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "SoulsGame/SUtils.h"
#include "SoulsGame/Abilities/SGAbilitySystemGlobals.h"
#include "SoulsGame/Character/MyPlayerController.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/Subsystems/UISubsystem.h"
#include "SoulsGame/UI/HUDWidget.h"


UMyAttributeSet::UMyAttributeSet()
    : Health(0.0f), MaxHealth(-1.0f), Mana(0.0f), MaxMana(-1.0f), AttackPowerAdditive(0.0f), Damage(0.0f)
{
    
}


void UMyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetMaxHealthAttribute())
    {
        if (MaxHealth.GetCurrentValue() == -1.0f)
        {
            MaxHealth.SetCurrentValue(NewValue);
            Health.SetCurrentValue(NewValue);
            Health.SetBaseValue(NewValue);
        }
        else
        {
            this->KeepSamePercentageAfterMaxChange(this->Health, this->MaxHealth, NewValue, GetHealthAttribute());
        }
    }
    else if (Attribute == GetMaxManaAttribute())
    {
        if (MaxMana.GetCurrentValue() == -1.0f)
        {
            MaxMana.SetCurrentValue(NewValue);
            Mana.SetCurrentValue(NewValue);
            Mana.SetBaseValue(NewValue);
        }
        else
        {
            this->KeepSamePercentageAfterMaxChange(this->Mana, this->MaxMana, NewValue, GetManaAttribute());
        }
    }
}

void UMyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
    UAbilitySystemComponent * Source = Context.GetOriginalInstigatorAbilitySystemComponent();
    const FGameplayTagContainer & SourceTags =  *(Data.EffectSpec.CapturedSourceTags.GetAggregatedTags());

    AActor * TargetActor = Data.Target.GetAvatarActor();
    AActor* SourceActor = Source->GetAvatarActor();

    ASCharacterBase * TargetCharacter = Cast<ASCharacterBase>(TargetActor);
    ASCharacterBase * SourceCharacter = Cast<ASCharacterBase>(SourceActor);
    
    float DeltaValue = 0;
    if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
    {
        DeltaValue = Data.EvaluatedData.Magnitude;
    }

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        const float MaxHealthVal = GetMaxHealth();
        const float CurHealthVal = GetHealth();
        const float NewHealthVal = FMath::Clamp(CurHealthVal, 0.0f, MaxHealthVal);
        SetHealth(NewHealthVal);
        if (TargetCharacter)
        {
            TargetCharacter->HandleHealthChanged(DeltaValue, SourceTags);
        }
    }
    else if (Data.EvaluatedData.Attribute == GetDamageAttribute())
    {
        
        if (Context.GetEffectCauser())
        {
            SourceActor = Context.GetEffectCauser();
        }

        // Do a hit result
        FHitResult HitResult;
        if (Context.GetHitResult())
        {
            HitResult = *Context.GetHitResult();
        }

        const float LocalDamageDone = GetDamage();
        SetDamage(0.0f);

        if (LocalDamageDone > 0)
        {
            const float OldHealth = GetHealth();
            const float NewHealth = FMath::Clamp(OldHealth - LocalDamageDone, 0.0f, GetMaxHealth());
            SetHealth(NewHealth);

            //UE_LOG(LogTemp, Warning, TEXT("OldHealth: %f , NewHealth %f"), OldHealth, NewHealth);
            if (TargetCharacter)
            {
                TargetCharacter->HandleDamage(LocalDamageDone, HitResult, SourceTags, SourceActor);
                TargetCharacter->HandleHealthChanged(-LocalDamageDone, SourceTags);
            }

            // Update boss health
            if (TargetCharacter->ActorHasTag(USGAbilitySystemGlobals::GetSG().Tag_CharacterType_AI_Boss.GetTagName()))
            {
                if (USGUISubsystem * Subsystem = FSUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
                {
                    if (UHUDWidget* HUD = Subsystem->GetHUDInstance())
                    {
                        if (NewHealth > 0)
                        {
                            HUD->UpdateBossHealth();
                        }
                        else
                        {
                            if (AMyPlayerController* Controller = Cast<AMyPlayerController>(HUD->GetOwningPlayer()))
                            {
                                Controller->UnsetBossActor();
                            }
                        }
                    }
                }
            }

        }
    }
    else
    {
        // UE_LOG(LogTemp, Warning, TEXT("Unknown attribute!"));
    }

}

void UMyAttributeSet::BeginDestroy()
{
    Super::BeginDestroy();
}


// Replicated props ================================================

void UMyAttributeSet::GetLifetimeReplicatedProps(::TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UMyAttributeSet, Health);
    DOREPLIFETIME(UMyAttributeSet, MaxHealth);
    DOREPLIFETIME(UMyAttributeSet, AttackPowerAdditive);
    DOREPLIFETIME(UMyAttributeSet, Health);
    
}

void UMyAttributeSet::OnRep_Level(const FGameplayAttributeData& OldLevel)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Level, OldLevel);
}

void UMyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Health, OldHealth);
}

void UMyAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxHealth, OldMaxHealth);
}

void UMyAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Mana, OldMana);
}

void UMyAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxMana, OldMaxMana);
}

void UMyAttributeSet::OnRep_AttackPowerAdditive(const FGameplayAttributeData& OldAttackPowerAdditive)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, AttackPowerAdditive, OldAttackPowerAdditive);
}

void UMyAttributeSet::OnRep_AttackPowerMultiplicative(const FGameplayAttributeData& OldAttackPowerMultiplicative)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, AttackPowerMultiplicative, OldAttackPowerMultiplicative);
}

void UMyAttributeSet::OnRep_Xp(const FGameplayAttributeData& OldXp)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Xp, OldXp);
}

void UMyAttributeSet::OnRep_MaxXp(const FGameplayAttributeData& OldMaxXp)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxXp, OldMaxXp);
}


// Helper functions =======================================================

void UMyAttributeSet::KeepSamePercentageAfterMaxChange(const FGameplayAttributeData& AffectedAttribute,
    const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const
{
    UAbilitySystemComponent * AbilityComp = GetOwningAbilitySystemComponent();
    const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
    if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
    {
        const float CurrentValue = AffectedAttribute.GetCurrentValue();
        const float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentMaxValue : NewMaxValue;
        AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
    }
}

/*
void UMyAttributeSet::GetAbilitySystemActorProperties(UAbilitySystemComponent* Component, AActor*& ReturnActor,
    AController*& ReturnController, ACharacterBase*& ReturnCharacter) const
{
    if (Component->AbilityActorInfo.IsValid() && Component->AbilityActorInfo->AvatarActor.IsValid())
    {
        ReturnActor = Component->AbilityActorInfo->AvatarActor.Get();
        ReturnController = Component->AbilityActorInfo->PlayerController.Get();
        if (ReturnController)
        {
            ReturnCharacter = Cast<ACharacterBase>(ReturnController->GetPawn());
        }
        else
        {
            ReturnCharacter = Cast<ACharacterBase>(ReturnActor);
        }
    }
}
*/