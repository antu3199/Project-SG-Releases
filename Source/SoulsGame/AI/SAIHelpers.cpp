#include "SAIHelpers.h"
#include "SoulsGame/Abilities/CharacterAbilitySystemComponent.h"
#include "SoulsGame/Abilities/MyGameplayAbility.h"


void UAIAbilityWrapper::Initialize(const FGameplayAbilitySpecHandle& SpecHandle)
{
    ExecuteAbilitySpecHandle = SpecHandle;
}

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

void UAIAbilityHistory::Initialize()
{
}

bool UAIAbilityHistory::HasLastQueuedAction() const
{
    if (AbilityHistoryItems.empty())
    {
        return false;
    }

    const FAbilityHistoryItem& LastItem = AbilityHistoryItems.top();
    return LastItem.HasQueuedAction();
}

FGameplayTagContainer UAIAbilityHistory::GetLastQueuedAction() const
{
    if (!HasLastQueuedAction())
    {
        return FGameplayTagContainer();
    }

     const FAbilityHistoryItem& LastItem = AbilityHistoryItems.top();
    return LastItem.QueuedAction;
}

void UAIAbilityWrapper_Legacy::Initialize(const FGameplayAbilitySpecHandle& SpecHandle, float InMinimumRangeToActivate,
                                          float InMoveToAcceptableRadius)
{
    AbilitySpecHandle = SpecHandle;
    MinimumRangeToActivate = InMinimumRangeToActivate;
    MoveToAcceptableRadius = InMoveToAcceptableRadius;
}

FGameplayAbilitySpec* UAIAbilityWrapper_Legacy::GetAbilitySpec(UCharacterAbilitySystemComponent* AbilitySystem) const
{
    if (AbilitySystem == nullptr)
    {
        return nullptr;
    }

    return AbilitySystem->FindAbilitySpecFromHandle(AbilitySpecHandle);
}

UMyGameplayAbility* UAIAbilityWrapper_Legacy::GetAbility(UCharacterAbilitySystemComponent* AbilitySystem) const
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
