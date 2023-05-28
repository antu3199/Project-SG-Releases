#include "SGProjectilePowerupAbility.h"

#include "SoulsGame/Character/SHumanoidPlayerCharacter.h"
#include "SoulsGame/Abilities/SGAbilitySystemGlobals.h"
#include "SoulsGame/Abilities/PowerUps/Components/SGProjectilePowerupComponent.h"
#include "SoulsGame/Character/MyPlayerController.h"

void USGProjectilePowerupAbility::PreActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                     const FGameplayEventData* TriggerEventData)
{
	Super::PreActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ProjectileParms.ShotComponent = MakeWeakObjectPtr(Cast<USGProjectilePowerupComponent>(PowerupComponent));
	ProjectileParms.ShotComponent->OnPostDoPowerup.AddDynamic(this, &USGProjectilePowerupAbility::OnShotFinished);
	ProjectileParms.ShotComponent->OnPowerupFinished.AddDynamic(this, &USGProjectilePowerupAbility::OnShootingFinished);
	ProjectileParms.ShotComponent->OnPreDoPowerup.AddDynamic(this, &USGProjectilePowerupAbility::OnPreShotHandle);
	ProjectileParms.ShotComponent->SetLevel(CurrentLevel);
	ShotsFinished = 0;
	
	bIsPlayerCharacter = ActorInfo->OwnerActor->GetInstigatorController()->IsPlayerController();
}

void USGProjectilePowerupAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ProjectileParms.ShotComponent.IsValid())
	{
		ProjectileParms.ShotComponent->OnPostDoPowerup.RemoveDynamic(this, &USGProjectilePowerupAbility::OnShotFinished);
		ProjectileParms.ShotComponent->OnPowerupFinished.RemoveDynamic(this, &USGProjectilePowerupAbility::OnShootingFinished);
		ProjectileParms.ShotComponent->OnPreDoPowerup.RemoveDynamic(this, &USGProjectilePowerupAbility::OnPreShotHandle);
	}

	ASHumanoidPlayerCharacter* PlayerCharacter =  Cast<ASHumanoidPlayerCharacter>(GetActorInfo().OwnerActor);
	if (PlayerCharacter != nullptr && PlayerCharacter->GetAbilitySystemComponent())
	{
		PlayerCharacter->GetAbilitySystemComponent()->RemoveGameplayEventTagContainerDelegate(GetStopShootingTagContainer(), EventDelegateHandle);
	}

	EventDelegateHandle.Reset();
	
}

bool USGProjectilePowerupAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
}

void USGProjectilePowerupAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}


void USGProjectilePowerupAbility::OnShotFinished()
{
}

void USGProjectilePowerupAbility::OnShootingFinished()
{
	ShotsFinished++;
	if (ShotsFinished >= 1)
	{
		this->EndAbility(this->CurrentSpecHandle, this->CurrentActorInfo, this->CurrentActivationInfo, true, false);
	}
}

void USGProjectilePowerupAbility::OnPreShotHandle()
{
	//USceneComponent* DefaultHomingTarget = ShootCharacter->GetDefaultHomingTarget();

	if (bIsPlayerCharacter)
	{
		if (APawn* Pawn = Cast<APawn>(ComponentOwner))
		{
			if (AMyPlayerController* Controller = Cast<AMyPlayerController>(Pawn->GetController()))
			{
				const FVector DefaultTarget = Controller->GetCameraForwardVector();
				ProjectileParms.ShotComponent->SetTargetVector(DefaultTarget);
			}
		}
	}
		
}

FGameplayTagContainer USGProjectilePowerupAbility::GetStopShootingTagContainer() const
{
	FGameplayTagContainer Container;
	Container.AddTag(GetStopShootingTag());
	return Container;
}

FGameplayTag USGProjectilePowerupAbility::GetStopShootingTag() const
{
	return USGAbilitySystemGlobals::GetSG().Tag_Triggers_Ability_Shoot_Stop;
}
