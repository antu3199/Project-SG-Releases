#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "UObject/Object.h"
#include <stack>

#include "SAIHelpers.generated.h"

enum class EAIWalkDirection : uint8
{
    Forwards,
    Left,
    Right,
    Backwards
};

UCLASS()
class SOULSGAME_API UAIAbilityWrapper : public UObject
{
    GENERATED_BODY()
public:
    void Initialize(const FGameplayAbilitySpecHandle & SpecHandle);

    FGameplayAbilitySpecHandle ExecuteAbilitySpecHandle;
    FGameplayAbilitySpecHandle QueuedAbilitySpecHandle;

    
    float MinimumActivationRange;
    float MaximumActivationRange;

    bool bDoWalk = false;
    EAIWalkDirection WalkDirection = EAIWalkDirection::Forwards;

    FGameplayAbilitySpec* GetAbilitySpec(class UCharacterAbilitySystemComponent* AbilitySystem ) const;
    class UMyGameplayAbility* GetAbilityCDO(class UCharacterAbilitySystemComponent* AbilitySystem ) const;
    // TODO: Maybe this should be a global function instead?
    class UMyGameplayAbility* GetAbilityInstance(class UCharacterAbilitySystemComponent* AbilitySystem) const;
    class UMyGameplayAbility* GetMostRecentAbilityInstance(class UCharacterAbilitySystemComponent* AbilitySystem) const;
};


struct FAbilityHistoryItem
{
    FGameplayTagContainer ExecutedAction;
    FGameplayTagContainer QueuedAction;

    bool HasQueuedAction() const
    {
        return !QueuedAction.IsEmpty();
    }
};


UCLASS()
class SOULSGAME_API UAIAbilityHistory : public UObject
{
    GENERATED_BODY()
public:
    void Initialize();

    bool HasLastQueuedAction() const;
    FGameplayTagContainer GetLastQueuedAction() const;
    
    std::stack<FAbilityHistoryItem> AbilityHistoryItems;
};



UCLASS()
class SOULSGAME_API UAIAbilityWrapper_Legacy : public UObject
{
    GENERATED_BODY()
public:
    void Initialize(const FGameplayAbilitySpecHandle & SpecHandle, float InMinimumRangeToActivate, float InMoveToAcceptableRadius);

    // Minimum distance to activate. For melee abilities, this should be -1. For range, it should be whatever else
    UPROPERTY()
    float MinimumRangeToActivate = -1.0f;

    // Distance from move to to acceptable radius
    UPROPERTY()
    float MoveToAcceptableRadius = -1.0f;

    FGameplayAbilitySpecHandle AbilitySpecHandle;

    FGameplayAbilitySpec* GetAbilitySpec(class UCharacterAbilitySystemComponent* AbilitySystem ) const;
    class UMyGameplayAbility* GetAbility(class UCharacterAbilitySystemComponent* AbilitySystem ) const;
    
};

class SAIHelpers
{
public:
    
};
