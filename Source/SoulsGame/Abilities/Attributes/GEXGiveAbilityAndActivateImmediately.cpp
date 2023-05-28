// Fill out your copyright notice in the Description page of Project Settings.


#include "GEXGiveAbilityAndActivateImmediately.h"
#include "MyAttributeSet.h"
#include "SoulsGame/Abilities/CharacterAbilitySystemComponent.h"


UGEXGiveAbilityAndActivateImmediately::UGEXGiveAbilityAndActivateImmediately() : Super()
{
    
}

void UGEXGiveAbilityAndActivateImmediately::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

    FPredictionKey PredictionKey = ExecutionParams.GetPredictionKey();
    FMath::SRandInit(PredictionKey.Current);

    UAbilitySystemComponent* Target = ExecutionParams.GetTargetAbilitySystemComponent();
    AActor* OwnerActor = Target->GetOwnerActor();
    UAbilitySystemComponent* Source = ExecutionParams.GetSourceAbilitySystemComponent();
    AActor* SourceActor = Source->GetOwnerActor();
    
    UCharacterAbilitySystemComponent* AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
    if (AbilitySystemComponent == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Ability system not found!"));
        return;
    }

    
    const FGameplayEffectSpec& OwningSpec = ExecutionParams.GetOwningSpec();
    const TArray<FGameplayAbilitySpecDef>& AbilitySpecs = OwningSpec.GrantedAbilitySpecs;
    for (auto& AbilitySpec : AbilitySpecs)
    {

        bool bAlreadyHasAbility = false;
        UGameplayAbility* AbilityCDO = AbilitySpec.Ability.GetDefaultObject();
        if (AbilityCDO == nullptr)
        {
            continue;
        }
        const TSubclassOf<UMyGameplayAbility>& Ability = (TSubclassOf<UMyGameplayAbility>)(AbilitySpec.Ability);

        TArray<UMyGameplayAbility *> ActiveAbilities;

        FGameplayTag AbilityTag = AbilityCDO->AbilityTags.First();
        AbilitySystemComponent->GetActiveAbilitiesWithTag(AbilityTag, ActiveAbilities);
        if (ActiveAbilities.Num() > 0)
        {
            AbilitySystemComponent->RemoveAbilityWithTag(AbilityTag);
        }

        FGameplayEventData GameplayEventData;
        GameplayEventData.Instigator = SourceActor;
        GameplayEventData.Target = OwnerActor;
        
        // Activate ability for first time
        AbilitySystemComponent->GrantAndActivateAbilityOnce(Ability, &GameplayEventData);

        /*
        if (FGameplayAbilitySpecWrapper* Wrapper = AbilitySystemComponent->GetGameplayWrapperWithTag(AbilityTag))
        {
            if (UMyGameplayAbility* InstancedAbility = Cast< UMyGameplayAbility>(Wrapper->GetPrimaryInstancedAbility()))
            {
                if (SourceActor != nullptr)
                {
                    InstancedAbility->SetOverrideInstigator(SourceActor);
                }
            }
        }
        ;
        */
        
        
    }
}
