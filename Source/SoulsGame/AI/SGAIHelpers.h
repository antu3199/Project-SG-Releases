#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "UObject/Object.h"
#include <stack>

#include "SGAIHelpers.generated.h"

enum class ESGAIWalkDirection : uint8
{
    Forwards,
    Left,
    Right,
    Backwards
};

UCLASS()
class SOULSGAME_API USGAIAbilityWrapper : public UObject
{
    GENERATED_BODY()
public:
    void Initialize(const FGameplayTag& Tag);

    FGameplayTag ExecuteAbilityTag;
    FGameplayTag QueuedAbilityTag;
    
    float MinimumActivationRange;
    float MaximumActivationRange;

    bool bDoWalk = false;
    ESGAIWalkDirection WalkDirection = ESGAIWalkDirection::Forwards;

    /*
    FGameplayAbilitySpec* GetAbilitySpec(class UCharacterAbilitySystemComponent* AbilitySystem ) const;
    class UMyGameplayAbility* GetAbilityCDO(class UCharacterAbilitySystemComponent* AbilitySystem ) const;
    // TODO: Maybe this should be a global function instead?
    class UMyGameplayAbility* GetAbilityInstance(class UCharacterAbilitySystemComponent* AbilitySystem) const;
    class UMyGameplayAbility* GetMostRecentAbilityInstance(class UCharacterAbilitySystemComponent* AbilitySystem) const;
    */
};


struct FSGAbilityHistoryItem
{
    FGameplayTag ExecutedAction;
    FGameplayTag QueuedAction;

    bool HasQueuedAction() const
    {
        return QueuedAction.IsValid();
    }
};


UCLASS()
class SOULSGAME_API USGAIAbilityHistory : public UObject
{
    GENERATED_BODY()
public:
    void Initialize();

    bool HasLastQueuedAction() const;
    FGameplayTag GetLastQueuedAction() const;
    
    std::stack<FSGAbilityHistoryItem> AbilityHistoryItems;
};

