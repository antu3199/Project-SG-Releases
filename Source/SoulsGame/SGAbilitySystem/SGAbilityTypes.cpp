#include "SGAbilityTypes.h"

#include "SGAbilityComponent.h"

USGAbilityComponent* FSGAbilityActorInfo::GetAbilityComponent() const
{
	if (OwnerActor.IsValid())
	{
		if (USGAbilityComponent* AbilityComponent = OwnerActor->FindComponentByClass<USGAbilityComponent>())
		{
			return AbilityComponent;
		}
	}

	return nullptr;
}

UAnimInstance* FSGAbilityActorInfo::GetAnimInstance() const
{
	if (!OwnerActor.IsValid())
	{
		return nullptr;
	}

	if (const USkeletalMeshComponent* SKMC = OwnerActor->FindComponentByClass<USkeletalMeshComponent>())
	{
		return SKMC->GetAnimInstance();
	}

	return nullptr;
}

void FSGAbilityActorInfo::InitFromActor(AActor* InOwnerActor, AActor* InAvatarActor)
{
	OwnerActor = MakeWeakObjectPtr(InOwnerActor);
	if (InAvatarActor != nullptr)
	{
		AvatarActor = MakeWeakObjectPtr(InAvatarActor);
	}
}

void FSGAbilityActorInfo::ClearActorInfo()
{
	OwnerActor.Reset();
	AvatarActor.Reset();
}

void FSGDamageParams::CalculateBaseDamage()
{
	if (RequestedFlatDamageCurve != nullptr)
	{
		FinalFlatDamage = RequestedFlatDamageCurve->GetFloatValue(Level);
	}
	else
	{
		FinalFlatDamage = RequestedFlatDamage;
	}

	if (RequestedAttackScalingCurve != nullptr)
	{
		FinalAttackScaling = RequestedAttackScalingCurve->GetFloatValue(Level);
	}
	else
	{
		FinalAttackScaling = RequestedAttackScaling;
	}

	bBaseDamageCalculated = true;
}

