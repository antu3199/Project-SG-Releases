
#include "SGUpgradeController.h"

#include "AbilitySystemComponent.h"
#include "SoulsGame/SGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/SUtils.h"
#include "SoulsGame/Abilities/SGAbilitySystemGlobals.h"
#include "SoulsGame/Abilities/PowerUps/SGProjectilePowerupAbility.h"


void FUpgradeOptionValue::SetTemplateValue(float RawValue)
{
	bHasTemplateValue = true;
	Value = RawValue;
	bRandRange = false;
}

void FUpgradeOptionValue::SetTemplateValueMinMax(float ValueMin, float ValueMax)
{
	bHasTemplateValue = true;
	ValueRandRangeMin = ValueMin;
	ValueRandRangeMax = ValueMax;
	Value = FMath::RandRange(ValueRandRangeMin, ValueRandRangeMax);
	bRandRange = true;
}

void FUpgradeOptionValue::CalculateValue()
{
	if (!bHasTemplateValue)
	{
		return;
	}
	
	bHasCalculatedValue = true;
	if (bRandRange)
	{
		Value = FMath::RandRange(ValueRandRangeMin, ValueRandRangeMax);
		// Round to 2 decimal places
		//Value = FMath::RoundToFloat(Value * 100) / 100;

		Value = FSUtils::RoundFloatToPrecision(Value, 2);
	}
}

float FUpgradeOptionValue::GetValue() const
{
	if (!bHasCalculatedValue)
	{
		UE_LOG(LogTemp, Warning, TEXT("Value is not in use!"));
	}

	return Value;
}

void FUpgradeOption::SetupOptionBeforeUse()
{
	Value1.CalculateValue();
	Value2.CalculateValue();
	Value3.CalculateValue();

	switch (UpgradeType)
	{
	case EUpgradeOptionType::DamageAdditive:
		OptionName = FString::Printf(TEXT("Increase damage by %s"), *FSUtils::GetFloatAsStringWithPrecision(Value1.GetValue(), 2));
		break;
	case EUpgradeOptionType::DamageMultiplicative:
		OptionName = FString::Printf(TEXT("Increase damage by %s%%."), *FSUtils::GetFloatAsStringWithPrecision(Value1.GetValue(), 2));
		break;
	case EUpgradeOptionType::UpgradeBasicShot:
		OptionName = FString::Printf(TEXT("Increase the number of shots in the Basic Shot."));
		break;
	case EUpgradeOptionType::FireWhip:
		OptionName = FString::Printf(TEXT("Fire whip go whoosh"));
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("[FUpgradeOption::SetupOptionFromType] Unable to identify option!"))
		break;
	}

	
}

USGUpgradeController* USGUpgradeController::GetSingleton(const UObject* WorldContextObject)
{
	if (USGameInstance* GameInstance = Cast<USGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject)))
	{
		return GameInstance->GetUpgradeController();
	}

	return nullptr;
}

void USGUpgradeController::InitializePool()
{
	// Create list of upgrade options here :)
	{
		UpgradePool.Add(FUpgradeOption());
		FUpgradeOption& NewOption = UpgradePool.Top();
		NewOption.UpgradeType = EUpgradeOptionType::DamageAdditive;
		NewOption.Value1.SetTemplateValueMinMax(50, 150);
	}

	{
		UpgradePool.Add(FUpgradeOption());
		FUpgradeOption& NewOption = UpgradePool.Top();
		NewOption.UpgradeType = EUpgradeOptionType::DamageMultiplicative;
		NewOption.Value1.SetTemplateValueMinMax(5, 20);
	}

	{
		// Upgrade basic shot
		UpgradePool.Add(FUpgradeOption());
		FUpgradeOption& NewOption = UpgradePool.Top();
		NewOption.UpgradeType = EUpgradeOptionType::UpgradeBasicShot;
	}

	{
		// Upgrade basic shot
		UpgradePool.Add(FUpgradeOption());
		FUpgradeOption& NewOption = UpgradePool.Top();
		NewOption.UpgradeType = EUpgradeOptionType::FireWhip;
	}
	
}

bool USGUpgradeController::GetNFromPool(const int32& N, TArray<FUpgradeOption>& OutUpgradeOptions, bool bAllowDuplicates) const
{
	if (bAllowDuplicates)
	{
		// If no duplicates, getting is a lot easier
		for (int32 i = 0; i < N; i++)
		{
			int32 RandIndex = FMath::RandRange(0, UpgradePool.Num() - 1);
			OutUpgradeOptions.Add(UpgradePool[RandIndex]);
		}
	}
	else
	{
		// If duplicates, copy the entire pool
		TArray<FUpgradeOption> CopiedUpgradePool = UpgradePool;

		for (int32 i = 0; i < N; i++)
		{
			int32 RandIndex = FMath::RandRange(0, CopiedUpgradePool.Num() - 1);
			OutUpgradeOptions.Add(CopiedUpgradePool[RandIndex]);
			CopiedUpgradePool.RemoveAt(RandIndex);
		}
	}


	if (OutUpgradeOptions.Num() != N)
	{
		UE_LOG(LogTemp, Error, TEXT("[USGUpgradeController::GetNFromPool] Unable to get N options from pool"));
		return false;
	}

	// Initialize out upgrades
	for (int32 i = 0; i < N; i++)
	{
		FUpgradeOption& UpgradeOption = OutUpgradeOptions[i];
		UpgradeOption.SetupOptionBeforeUse();
	}

	return true;
}

void USGUpgradeController::ApplyUpgradeToCharacter(const FUpgradeOption& Option, ASCharacterBase* Character)
{
	UCharacterAbilitySystemComponent* AbilitySystemComponent =Cast<UCharacterAbilitySystemComponent>( Character->GetAbilitySystemComponent());
	if (AbilitySystemComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ USGUpgradeController::ApplyUpgradeToCharacter] Unable to get ability system!"))
		return;
	}

	FUpgradeOption OptionToBroadcast = Option;

	// TODO: Refactor so that all upgrades are registered as gameplay effects - Only relevant for networking...
	if (Option.UpgradeType == EUpgradeOptionType::DamageAdditive)
	{
		UMyGameplayEffect* DynamicGameplayEffect = NewObject<UMyGameplayEffect>();
		
		DynamicGameplayEffect->Modifiers.Add(FGameplayModifierInfo());
		FGameplayModifierInfo& ModifierInfo = DynamicGameplayEffect->Modifiers.Top();
		ModifierInfo.Attribute = UMyAttributeSet::GetAttackPowerAdditiveAttribute();
		ModifierInfo.ModifierOp = EGameplayModOp::Type::Additive;

		FScalableFloat MagnitudeValue;
		MagnitudeValue.Value = Option.Value1.GetValue();
		ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(MagnitudeValue);

		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		AbilitySystemComponent->ApplyGameplayEffectToSelf(DynamicGameplayEffect, 0, EffectContext);

		OptionToBroadcast.DynamicGameplayEffect = DynamicGameplayEffect;
		
	}
	else if (Option.UpgradeType == EUpgradeOptionType::DamageMultiplicative)
	{
		UMyGameplayEffect* DynamicGameplayEffect = NewObject<UMyGameplayEffect>();
		
		DynamicGameplayEffect->Modifiers.Add(FGameplayModifierInfo());
		FGameplayModifierInfo& ModifierInfo = DynamicGameplayEffect->Modifiers.Top();
		ModifierInfo.Attribute = UMyAttributeSet::GetAttackPowerMultiplicativeAttribute();
		ModifierInfo.ModifierOp = EGameplayModOp::Type::Additive;

		FScalableFloat MagnitudeValue;
		MagnitudeValue.Value = Option.Value1.GetValue() / 100.0f; 
		ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(MagnitudeValue);
		OptionToBroadcast.DynamicGameplayEffect = DynamicGameplayEffect;
	}
	else if (Option.UpgradeType == EUpgradeOptionType::UpgradeBasicShot)
	{
		UCharacterAbilitySystemComponent* AbilitySystem = Cast<UCharacterAbilitySystemComponent>(Character->GetAbilitySystemComponent());
		FGameplayAbilitySpecWrapper* Wrapper = AbilitySystem->GetGameplayWrapperWithTag(USGAbilitySystemGlobals::GetSG().Tag_Ability_Shoot);
    
		USGProjectilePowerupAbility* Ability = Cast<USGProjectilePowerupAbility>(Wrapper->GetPrimaryInstancedAbility());
		if (Ability != nullptr)
		{
			int32 CurrentLevel = Ability->GetLevel();
			Ability->SetLevel(CurrentLevel + 1);
		}
	}
	else if (Option.UpgradeType == EUpgradeOptionType::FireWhip)
	{
		UMyGameplayAbility* Ability = GetGameplayAbilityFromGiveAbilityGETemplate(GiveFireWhipEffectTemplate);
		if (Ability == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("[USGUpgradeController::ApplyUpgradeToCharacter] Cannot find ability!"));
			return;
		}
		
		FGameplayTag AbilityTag = Ability->AbilityTags.GetByIndex(0);;
		UCharacterAbilitySystemComponent* AbilitySystem = Cast<UCharacterAbilitySystemComponent>(Character->GetAbilitySystemComponent());
		FGameplayAbilitySpecWrapper* Wrapper = AbilitySystem->GetGameplayWrapperWithTag(AbilityTag);
		USGPowerupAbility* InstancedAbility = nullptr;
		if (Wrapper != nullptr)
		{
			InstancedAbility = Cast<USGProjectilePowerupAbility>(Wrapper->GetPrimaryInstancedAbility());;
		}
		
		if (InstancedAbility == nullptr)
		{
			AbilitySystem->ApplyGameplayEffect(GiveFireWhipEffectTemplate, Character);
		}
		else
		{
			int32 CurrentLevel = InstancedAbility->GetLevel();
			InstancedAbility->SetLevel(CurrentLevel + 1);
		}
		
		
		//Effect->Grant
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[USGUpgradeController::ApplyUpgradeToCharacter] Unable to identify option!"))
	}

	OnUpgradeAppliedDelegate.Broadcast(OptionToBroadcast, Character);

}

UMyGameplayAbility* USGUpgradeController::GetGameplayAbilityFromGiveAbilityGETemplate(
	TSubclassOf<UMyGameplayEffect> Template) const
{
	UMyGameplayEffect* Effect = Cast<UMyGameplayEffect>(Template->GetDefaultObject());
	if (Effect == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[USGUpgradeController::GetGameplayAbilityFromGiveAbilityGETemplate] Cannot find effect!"));
		return nullptr;
	}

	if (Effect->GrantedAbilities.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[USGUpgradeController::GetGameplayAbilityFromGiveAbilityGETemplate] Not a granted ability"));
		return nullptr;
	}

	UMyGameplayAbility* Ability = Cast<UMyGameplayAbility>(Effect->GrantedAbilities[0].Ability.GetDefaultObject());
	if (Ability == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[USGUpgradeController::GetGameplayAbilityFromGiveAbilityGETemplate] Cannot find ability"));
		return nullptr;
	}

	return Ability;
}

FGameplayTag USGUpgradeController::GetGameplayTagFromGiveAbilityGETemplate(TSubclassOf<UMyGameplayEffect> Template) const
{
	UMyGameplayAbility* Ability = GetGameplayAbilityFromGiveAbilityGETemplate(Template);
	if (Ability == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[USGUpgradeController::GetGameplayTagFromGiveAbilityGETemplate] Cannot find ability!"));
		return FGameplayTag();
	}

	if (Ability->AbilityTags.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[USGUpgradeController::GetGameplayTagFromGiveAbilityGETemplate] No ability tag"));
		return FGameplayTag();
	}
	
	return Ability->AbilityTags.GetByIndex(0);
}
