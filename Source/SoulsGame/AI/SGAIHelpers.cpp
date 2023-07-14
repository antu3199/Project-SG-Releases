#include "SoulsGame/AI/SGAIHelpers.h"


void USGAIAbilityWrapper::Initialize(const FGameplayTag& Tag)
{
    ExecuteAbilityTag = Tag;
}

/*
FGameplayAbilitySpec* UAIAbilityWrapper::GetAbilitySpec(UCharacterAbilitySystemComponent* AbilitySystem) const
{
    if (AbilitySystem == nullptr)
    {
        return nullptr;
    }

    return AbilitySystem->FindAbilitySpecFromHandle(ExecuteAbilitySpecHandle);
}

UMyGameplayAbility* UAIAbilityWrapper::GetAbilityCDO(UCharacterAbilitySystemComponent* AbilitySystem) const
{
    if (AbilitySystem == nullptr)
    {
        return nullptr;
    }

    FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec(AbilitySystem);
    if (AbilitySpec == nullptr)
    {
        return nullptr;
    }

    return Cast<UMyGameplayAbility>(AbilitySpec->Ability);
}

UMyGameplayAbility* UAIAbilityWrapper::GetAbilityInstance(UCharacterAbilitySystemComponent* AbilitySystem) const
{
    UMyGameplayAbility* AbilityCDO = GetAbilityCDO(AbilitySystem);
    if (AbilityCDO == nullptr)
    {
        return nullptr;
    }
    
    FGameplayAbilitySpecWrapper* Wrapper = AbilitySystem->GetGameplayWrapperWithTag(AbilityCDO->AbilityTags);
    return Cast<UMyGameplayAbility>(Wrapper->GetPrimaryInstancedAbility());
}

UMyGameplayAbility* UAIAbilityWrapper::GetMostRecentAbilityInstance(
    UCharacterAbilitySystemComponent* AbilitySystem) const
{
    UMyGameplayAbility* AbilityCDO = GetAbilityCDO(AbilitySystem);
    if (AbilityCDO == nullptr)
    {
        return nullptr;
    }
    
    FGameplayAbilitySpecWrapper* Wrapper = AbilitySystem->GetGameplayWrapperWithTag(AbilityCDO->AbilityTags);
    return Cast<UMyGameplayAbility>(Wrapper->GetMostRecentInstancedAbility());
}
*/

void USGAIAbilityHistory::Initialize()
{
}

bool USGAIAbilityHistory::HasLastQueuedAction() const
{
    if (AbilityHistoryItems.empty())
    {
        return false;
    }

    const FSGAbilityHistoryItem& LastItem = AbilityHistoryItems.top();
    return LastItem.HasQueuedAction();
}

FGameplayTag USGAIAbilityHistory::GetLastQueuedAction() const
{
    if (!HasLastQueuedAction())
    {
        return FGameplayTag();
    }

     const FSGAbilityHistoryItem& LastItem = AbilityHistoryItems.top();
    return LastItem.QueuedAction;
}

