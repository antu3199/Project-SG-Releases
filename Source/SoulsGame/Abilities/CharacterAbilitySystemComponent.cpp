// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAbilitySystemComponent.h"

#include "GameplayTagsManager.h"
#include "MyGameplayAbility.h"

// UCharacterAbilitySystemComponent =====================================

// Gets activatableAbilities with tags
void UCharacterAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer & GameplayTagContainer, OUT TArray<UMyGameplayAbility *>& ActiveAbilities)
{
    // Get all abilities matching the gameplay tag container
    TArray<FGameplayAbilitySpec *> AbilitiesToActivate;
    GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, AbilitiesToActivate, false);

    for (FGameplayAbilitySpec * Spec : AbilitiesToActivate)
    {
        // Iterate all instances on this ability spec
        // Note that there can be two abilities with the same name
        // e.g. two abilities named "melee", with one requiring unarmed and one requiring weapon.
        TArray<UGameplayAbility*> AbilityInstances = Spec->GetAbilityInstances();

        for (UGameplayAbility * ActiveAbility : AbilityInstances)
        {
            ActiveAbilities.Add(Cast<UMyGameplayAbility>(ActiveAbility));
        }
    }
}

void UCharacterAbilitySystemComponent::GetActiveAbilitiesWithTag(const FString & TagName,
    TArray<UMyGameplayAbility*>& ActiveAbilities)
{
    FGameplayTagContainer TagContainer;
    const FGameplayTag Tag = UGameplayTagsManager::Get().RequestGameplayTag(FName(TagName), true);
    GetActiveAbilitiesWithTag(Tag, ActiveAbilities);
}

void UCharacterAbilitySystemComponent::GetActiveAbilitiesWithTag(const FGameplayTag& Tag,
    TArray<UMyGameplayAbility*>& ActiveAbilities)
{
    FGameplayTagContainer TagContainer;
    TagContainer.AddTag(Tag);

    this->GetActiveAbilitiesWithTags(TagContainer, ActiveAbilities);
}

bool  UCharacterAbilitySystemComponent::IsUsingAbilityWithTag(const FString & Tag)
{
    // ReSharper disable once IdentifierTypo
    TArray<UMyGameplayAbility*> MyAbilities;
    this->GetActiveAbilitiesWithTag(Tag, MyAbilities);
    return MyAbilities.Num() > 0;
}

bool UCharacterAbilitySystemComponent::ActivateAbilityWithTag(const FString & TagName)
{
    const FGameplayTag Tag =  UGameplayTagsManager::Get().RequestGameplayTag(FName(TagName));
    return ActivateAbilityWithTag(Tag);
}

bool UCharacterAbilitySystemComponent::ActivateAbilityWithTag(const FGameplayTag& Tag)
{
    const TArray<FGameplayAbilitySpecWrapper> & ContainerArray = this->GetGameplaySpecWrappers();
	
    for (int i = 0; i < ContainerArray.Num(); i++)
    {
        const FGameplayAbilitySpecWrapper & Container = ContainerArray[i];

        if (Container.HasTag(Tag))
        {
            return Container.ActivateAbility();
        }
    }

    return false;
}


void UCharacterAbilitySystemComponent::ApplyGameplayEffect(
    TSubclassOf<UMyGameplayEffect>& Effect, const UObject* NewSourceObject)
{
    const int level = 1;	
    FGameplayEffectContextHandle EffectContext = this->MakeEffectContext();
    EffectContext.AddSourceObject(NewSourceObject);
    const FGameplayEffectSpecHandle NewHandle = this->MakeOutgoingSpec(Effect, level, EffectContext);
    if (NewHandle.IsValid())
    {
        this->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), this);
    }

}

FGameplayAbilitySpecWrapper& UCharacterAbilitySystemComponent::GrantAbility(const TSubclassOf<UMyGameplayAbility>& Ability)
{
    FGameplayAbilitySpecWrapper GameplaySpec;
    GameplaySpec.Initialize(this);
    GameplaySpec.GiveAbility(Ability);
    
    GameplaySpecWrappers.Add(GameplaySpec);

    UE_LOG(LogTemp, Warning, TEXT("Ability granted %s"), *GameplaySpec.GetAbilityName());
    return GameplaySpecWrappers.Last();
}

bool UCharacterAbilitySystemComponent::GrantAndActivateAbilityOnce(const TSubclassOf<UMyGameplayAbility>& Ability, const FGameplayEventData* GameplayEventData)
{
    FGameplayAbilitySpecWrapper GameplaySpec;
    GameplaySpec.Initialize(this);
    GameplaySpec.GiveAbilityAndActivateOnce(Ability, GameplayEventData);
    GameplaySpecWrappers.Add(GameplaySpec);

    return true;
}

/*
void UCharacterAbilitySystemComponent::GrantAbility(UMyGameplayAbility* Ability)
{
    FGameplayAbilitySpecWrapper GameplaySpec(Ability);
    GameplaySpec.GiveAbility(this);
    
    GameplaySpecWrappers.Add(GameplaySpec);

    UE_LOG(LogTemp, Warning, TEXT("Ability granted %s"), *GameplaySpec.GetAbilityName());
}
*/
void UCharacterAbilitySystemComponent::GrantAbilities(const TArray<TSubclassOf<UMyGameplayAbility>>& Abilities)
{
    for (auto & Ability : Abilities)
    {
        GrantAbility(Ability);
    }
}

bool UCharacterAbilitySystemComponent::IsUsingAnyAbility()
{
    for (auto & Wrapper : GameplaySpecWrappers)
    {
        FGameplayAbilitySpec * Spec = Wrapper.GetAbilitySpec();
        if (!Spec) continue;
        
        if (Spec->IsActive())
        {
            return true;
        }
    }
    
    return false;
}

void UCharacterAbilitySystemComponent::RemoveAbilityWithTag(const FGameplayTag& Tag)
{
    FGameplayAbilitySpecWrapper* Wrapper = GetGameplayWrapperWithTag(Tag);
    if (Wrapper == nullptr)
    {
        return;
    }

    UGameplayAbility* Ability = Wrapper->GetPrimaryInstancedAbility();
    if (Ability != nullptr)
    {
        Ability->K2_CancelAbility();
    }

    int32 RemoveIndex = -1;
    for (int32 i = 0; i < GameplaySpecWrappers.Num(); i++)
    {
        FGameplayAbilitySpecWrapper& Spec = GameplaySpecWrappers[i];
        if (Spec.GameplayAbilitySpecHandle == Wrapper->GameplayAbilitySpecHandle)
        {
            RemoveIndex = i;
            break;
        }
    }

    if (RemoveIndex != -1)
    {
        GameplaySpecWrappers.RemoveAt(RemoveIndex);
    }
}

FGameplayAbilitySpecWrapper* UCharacterAbilitySystemComponent::GetGameplayWrapperWithTag(const FGameplayTag& Tag)
{
    for (auto& GameplaySpecWrapper : GameplaySpecWrappers)
    {
        if (GameplaySpecWrapper.HasTag(Tag))
        {
            return &GameplaySpecWrapper;
        }
    }

    return nullptr;
    
}

FGameplayAbilitySpecWrapper* UCharacterAbilitySystemComponent::GetGameplayWrapperWithTag(
    const FGameplayTagContainer& Tag)
{
    for (auto& GameplaySpecWrapper : GameplaySpecWrappers)
    {
        if (GameplaySpecWrapper.HasAllTags(Tag))
        {
            return &GameplaySpecWrapper;
        }
    }

    return nullptr;
}

void UCharacterAbilitySystemComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UCharacterAbilitySystemComponent::BeginDestroy()
{
    Super::BeginDestroy();
}

void UCharacterAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability,
    bool bWasCancelled)
{
    Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

    int32 RemoveIndex = -1;
    for (int32 i = 0; i < GameplaySpecWrappers.Num(); i++)
    {
        FGameplayAbilitySpecWrapper& Wrapper = GameplaySpecWrappers[i];
        if (Wrapper.bRemoveAbilityOnFinish && Handle == Wrapper.GameplayAbilitySpecHandle)
        {
            RemoveIndex = i;
            break;
        }
    }

    if (RemoveIndex != -1)
    {
        GameplaySpecWrappers.RemoveAt(RemoveIndex);
    }
}


