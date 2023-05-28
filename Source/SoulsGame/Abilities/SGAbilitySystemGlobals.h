#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "GameplayTagsManager.h"
//#include "GameplayTagsManager.h"
#include "SGAbilitySystemGlobals.generated.h"

/**
 * Holds global data for the Ability System.
 */
UCLASS(config=Game)
class SOULSGAME_API USGAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	USGAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer);

	static USGAbilitySystemGlobals& GetSG()
	{
		return *Cast<USGAbilitySystemGlobals>(&Get());
	}

	UPROPERTY()
	FGameplayTag Tag_Ability1 =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Ability.Ability1"));
	UPROPERTY()
	FGameplayTag Tag_Ability2 =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Ability.Ability2"));
	UPROPERTY()
	FGameplayTag Tag_Ability3 =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Ability.Ability3"));
	UPROPERTY()
	FGameplayTag Tag_Ability4 =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Ability.Ability4"));

	UPROPERTY()
	FGameplayTag Tag_Ability_UseItem =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Ability.UseItem"));
	
	UPROPERTY()
	FGameplayTag Tag_Ability_Dash =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Ability.Dash"));

	UPROPERTY()
	FGameplayTag Tag_Ability_Backstep =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Ability.Backstep"));

	UPROPERTY()
	FGameplayTag Tag_Ability_Parry =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Ability.Parry"));
	
	UPROPERTY()
	FGameplayTag Tag_Triggers_Ability_Activate =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Triggers.Ability.Activate"));

	UPROPERTY()
	FGameplayTag Tag_Ability_Shoot =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Ability.Shoot"));
	
	UPROPERTY()
	FGameplayTag Tag_Triggers_Ability_Shoot_Stop =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Triggers.Ability.Shoot.Stop"));
	UPROPERTY()
	FGameplayTag Tag_Triggers_Character_SetHitboxEffect =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Triggers.Character.SetHitboxEffect"));

	UPROPERTY()
	FGameplayTag Tag_AbilityType_Melee =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("AbilityType.Melee"));

	UPROPERTY()
	FGameplayTag Tag_AbilityType_Ranged =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("AbilityType.Ranged"));

	UPROPERTY()
	FGameplayTag Tag_AbilityType_Self =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("AbilityType.Self"));

	
	UPROPERTY()
	FGameplayTag Tag_State_Invincible =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("State.Invincible"));
	
	UPROPERTY()
	FGameplayTag Tag_State_Stun =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("State.Stun"));

	UPROPERTY()
	FGameplayTag Tag_State_Timelock =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("State.Timelock"));

	UPROPERTY()
	FGameplayTag Tag_Triggers_Character_CheckEmpowered_Enable =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Triggers.Character.CheckEmpowered.Enable"));
	
	UPROPERTY()
	FGameplayTag Tag_Triggers_Character_CheckEmpowered_Disable =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Triggers.Character.CheckEmpowered.Disable"));

	UPROPERTY()
	FGameplayTag Tag_State_Countering =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("State.Countering"));

	
	// Maybe doesn't belong here, but I lazy
	UPROPERTY()
	FGameplayTag Tag_CharacterType_AI_Boss =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("CharacterType.AI.Boss"));

	UPROPERTY()
	FGameplayTag Tag_Triggers_DataObject =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Triggers.DataObject"));

	UPROPERTY()
	FGameplayTag Tag_AbilityEvent_AbilityBroadcasts_MontageEnd = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Ability.AbilityBroadcasts.MontageEnd")); 

	//~ Begin UAbilitySystemGlobals interface
protected:

	virtual void InitGlobalTags() override;
	//~ End UAbilitySystemGlobals interface
};