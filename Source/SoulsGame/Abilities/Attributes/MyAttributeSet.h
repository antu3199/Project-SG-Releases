// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "MyAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
class ASCharacterBase;

UCLASS()
class SOULSGAME_API UMyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
    UMyAttributeSet();

    
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

    virtual void BeginDestroy() override;

    virtual void GetLifetimeReplicatedProps(::TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(BlueprintReadOnly, Category = "Level", ReplicatedUsing = OnRep_Level)
    FGameplayAttributeData Level;
    ATTRIBUTE_ACCESSORS(UMyAttributeSet, Level)

    UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UMyAttributeSet, Health)

    UPROPERTY(BlueprintReadOnly, Category = "MaxHealth", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxHealth)

    UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_Mana)
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS(UMyAttributeSet, Mana)

    UPROPERTY(BlueprintReadOnly, Category = "MaxMana", ReplicatedUsing = OnRep_MaxMana)
    FGameplayAttributeData MaxMana;
    ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxMana)

    
    UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_AttackPowerAdditive)
    FGameplayAttributeData AttackPowerAdditive;
    ATTRIBUTE_ACCESSORS(UMyAttributeSet, AttackPowerAdditive)

    UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_AttackPowerMultiplicative)
    FGameplayAttributeData AttackPowerMultiplicative;
    ATTRIBUTE_ACCESSORS(UMyAttributeSet, AttackPowerMultiplicative)
    
    // Damage is a 'temporary' attribute used by the DamageExecution to calculate final damage, which then turns into -Health 
    // Temporary value that only exists on the Server. Not replicated.
    UPROPERTY(BlueprintReadOnly, Category = "Damage", meta = (HideFromLevelInfos))
    FGameplayAttributeData Damage;
    ATTRIBUTE_ACCESSORS(UMyAttributeSet, Damage)

    UPROPERTY(BlueprintReadOnly, Category = "Xp", ReplicatedUsing = OnRep_Xp)
    FGameplayAttributeData Xp;
    ATTRIBUTE_ACCESSORS(UMyAttributeSet, Xp)

    UPROPERTY(BlueprintReadOnly, Category = "Xp", ReplicatedUsing = OnRep_MaxXp)
    FGameplayAttributeData MaxXp;
    ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxXp)
    

    // Note: Not sure if need to apply mod to attribute if only local game ...
    void MultiplyStats(float Amount)
    {
        float NewMaxHealth = this->GetMaxHealth() * Amount;
        this->KeepSamePercentageAfterMaxChange(this->Health, this->MaxHealth, NewMaxHealth, GetHealthAttribute());

        float NewMaxMana = this->GetMaxMana() * Amount;
        this->KeepSamePercentageAfterMaxChange(this->Mana, this->MaxMana, NewMaxMana, GetManaAttribute());

        
        SetAttackPowerAdditive(GetAttackPowerAdditive() * Amount);
        SetAttackPowerMultiplicative(GetAttackPowerMultiplicative() * Amount);
    }
    
protected:

    UFUNCTION()
    virtual void OnRep_Level(const FGameplayAttributeData & OldLevel);
    
    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData & OldHealth);

    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData & OldMaxHealth);

    UFUNCTION()
    virtual void OnRep_Mana(const FGameplayAttributeData & OldMana);

    UFUNCTION()
    virtual void OnRep_MaxMana(const FGameplayAttributeData & OldMaxMana);
    
    
    UFUNCTION()
    virtual void OnRep_AttackPowerAdditive(const FGameplayAttributeData & OldAttackPowerAdditive);

    UFUNCTION()
    virtual void OnRep_AttackPowerMultiplicative(const FGameplayAttributeData & OldAttackPowerMultiplicative);

    UFUNCTION()
    virtual void OnRep_Xp(const FGameplayAttributeData & OldXp);

    UFUNCTION()
    virtual void OnRep_MaxXp(const FGameplayAttributeData & OldMaxXp);
    
    
    // Helper function to keep same percentage after applying change to max value of an attribute

    void KeepSamePercentageAfterMaxChange(const FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const;

    //void GetAbilitySystemActorProperties(UAbilitySystemComponent * Component, AActor *& ReturnActor, AController *& ReturnController, ACharacterBase *& ReturnCharacter) const;
};
