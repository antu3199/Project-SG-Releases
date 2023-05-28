#include "SGAbilitySystemGlobals.h"

#include "GameplayTagsManager.h"

USGAbilitySystemGlobals::USGAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USGAbilitySystemGlobals::InitGlobalTags()
{
	Super::InitGlobalTags();

	//InvincibleTag =  UGameplayTagsManager::Get().RequestGameplayTag(TEXT("State.Invincible"));
}
