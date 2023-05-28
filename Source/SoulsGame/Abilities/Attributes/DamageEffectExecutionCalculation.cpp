// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageEffectExecutionCalculation.h"
#include "MyAttributeSet.h"

struct MyDamageStatics
{
    // DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower);
    DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPowerAdditive);
    DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPowerMultiplicative);
    DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

    MyDamageStatics()
    {
        // DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, DefensePower, Target, false);
        // Bool is in/out snapshot
        DEFINE_ATTRIBUTE_CAPTUREDEF(UMyAttributeSet, AttackPowerAdditive, Source, true);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UMyAttributeSet, AttackPowerMultiplicative, Source, true);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UMyAttributeSet, Damage, Source, true);
    }
    
};

static const MyDamageStatics& DamageStatics()
{
    static MyDamageStatics DmgStatics;
    return DmgStatics;
}

UDamageEffectExecutionCalculation::UDamageEffectExecutionCalculation()
{
    RelevantAttributesToCapture.Add(DamageStatics().AttackPowerAdditiveDef);
    RelevantAttributesToCapture.Add(DamageStatics().AttackPowerMultiplicativeDef);
    RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
}

void UDamageEffectExecutionCalculation::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

    FPredictionKey PredictionKey = ExecutionParams.GetPredictionKey();
    FMath::SRandInit(PredictionKey.Current);
    
    UAbilitySystemComponent * TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
    UAbilitySystemComponent * SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

    AActor * SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
    AActor * TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;

    const FGameplayEffectSpec & Spec = ExecutionParams.GetOwningSpec();

    // Gather the tags from the source and target as that can affect which buffs should be used
    const FGameplayTagContainer * SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer * TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    /*
    float DefensePower = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefensePowerDef, EvaluationParameters, DefensePower);
    if (DefensePower == 0.0f)
    {
        DefensePower = 1.0f;
    }
     */
    //

    // Gets the ability given the effect context
   //const UGameplayAbility* TestAbility = ExecutionParams.GetOwningSpec().GetEffectContext().GetAbilityInstance_NotReplicated();

    
    float AttackPowerAdditive = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerAdditiveDef, EvaluationParameters, AttackPowerAdditive );

    float AttackPowerMultiplicative = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerMultiplicativeDef, EvaluationParameters, AttackPowerMultiplicative );

    
    float Damage = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage );

    //float DamageDone = Damage * AttackPower / DefensePower;
    const float DamageDone = (Damage + AttackPowerAdditive) * (1 + AttackPowerMultiplicative);
    
    if (DamageDone > 0.f)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, DamageDone));
    }
}
