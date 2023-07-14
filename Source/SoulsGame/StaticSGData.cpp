#include "StaticSGData.h"

FGameplayTag FSGStaticRelicItem::GetAbilityTag() const
{
	if (const USGAbility* AbilityCDO = Cast<USGAbility>(AbilityClass->GetDefaultObject()))
	{
		return AbilityCDO->GetIdentifierTag();
	}

	return FGameplayTag();
}

FString UStaticSGData::GetSGVersion() const
{
	return GetVersion();
}

FString UStaticSGData::GetVersion()
{
	return SG_VERSION;
}
