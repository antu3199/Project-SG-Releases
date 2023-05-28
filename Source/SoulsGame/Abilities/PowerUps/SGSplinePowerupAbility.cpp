#include "SGSplinePowerupAbility.h"

#include "SoulsGame/Abilities/SGAbilitySystemGlobals.h"
#include "SoulsGame/Abilities/PowerUps/Components/SGProjectilePowerupComponent_SpawnSplineActor.h"

void USGSplinePowerupAbility::PreActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	Super::PreActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AbilityParms.SplineComponent = MakeWeakObjectPtr(Cast<USGSpawnSplineActorPowerupComponent>(PowerupComponent));
	AbilityParms.SplineComponent->OnPostDoPowerup.AddDynamic(this, &USGSplinePowerupAbility::OnSplineFinished);
	// ProjectileParms.ShotFinishedHandle = ProjectileParms.ShotComponent->OnShotFinished.AddUObject(this, &USGProjectilePowerupAbility::OnShotFinished);
	// ProjectileParms.ShootingFinishedHandle = ProjectileParms.ShotComponent->OnShootingFinished.AddUObject(this, &USGProjectilePowerupAbility::OnShootingFinished);
	// ProjectileParms.PreShotHandle = ProjectileParms.ShotComponent->OnPreShot.AddUObject(this, &USGProjectilePowerupAbility::OnPreShotHandle);
	// ProjectileParms.ShotComponent->SetLevel(CurrentLevel);
	// ShotsFinished = 0;
	
}

void USGSplinePowerupAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	AbilityParms.SplineComponent->OnPostDoPowerup.RemoveDynamic(this, &USGSplinePowerupAbility::OnSplineFinished);
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USGSplinePowerupAbility::OnSplineFinished()
{
	
	//this->EndAbility(this->CurrentSpecHandle, this->CurrentActorInfo, this->CurrentActivationInfo, true, false);
}

