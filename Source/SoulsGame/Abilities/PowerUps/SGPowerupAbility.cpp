#include "SGPowerupAbility.h"

#include "GameplayTagsManager.h"
#include "SoulsGame/Abilities/SGAbilitySystemGlobals.h"
#include "Kismet/GameplayStatics.h"
#include "SoulsGame/Character/SHumanoidPlayerCharacter.h"
#include "SoulsGame/Character/SProjectileShooterCharacter.h"
#include "SoulsGame/Weapon/AbilityActor.h"

void USGPowerupAbility::SetLevel(int32 Level)
{
    Super::SetLevel(Level);
    if (PowerupComponent != nullptr)
    {
        PowerupComponent->SetLevel(Level);
    }
}

USGPowerupComponent* USGPowerupAbility::GetPowerupComponent() const
{
    return PowerupComponent;
}

const TOptional<FGameplayEventData>& USGPowerupAbility::GetGameplayEventData() const
{
    return GameplayEventDataPtr;
}

void USGPowerupAbility::DoActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::DoActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    if (bAutoActivateComponent)
    {
        PowerupComponent->OnActivateAbility(ActorInfo, ActivationInfo, TriggerEventData);
    }
}

void USGPowerupAbility::InitializeBase(const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo& ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    ComponentOwner = ActorInfo->OwnerActor.Get();

    if (ComponentOwner != nullptr)
    {
        // Assmume only one component at a time.
        // If need more than one, then need to enable USGPowerupComponent->bDetachOnEndAbility
        USGPowerupComponent* ExistingComponent = Cast<USGPowerupComponent>(ComponentOwner->GetComponentByClass(PowerupComponentTemplate));
        if (ExistingComponent != nullptr)
        {
            PowerupComponent = ExistingComponent;
            if (ExistingComponent->GetDetachOnEndAbility())
            {
                UE_LOG(LogTemp, Warning, TEXT("Assmume only one component at a time. If need more than one, then need to enable USGPowerupComponent->bDetachOnEndAbility"));
            }
        }
        else
        {
            PowerupComponent = NewObject<USGPowerupComponent>(ComponentOwner, PowerupComponentTemplate);
            PowerupComponent->RegisterComponent();
        }

        // Need to reinitialize each time, to make sure have the most recent stuff
        PowerupComponent->OnInitializeAbilityContext(this, ActorInfo->OwnerActor.Get(), ComponentOwner);


    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[USGPowerupAbility::OnGiveAbility] Unable to create component"));
    }

    if (TriggerEventData != nullptr)
    {
        FGameplayEventData Data = *TriggerEventData;
        GameplayEventDataPtr.Emplace(Data);
    }
}

/*
void USGPowerupAbility::OnCommitAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
    Super::OnCommitAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    OnInitialize(ActorInfo, ActivationInfo, TriggerEventData);

    if (TriggerEventData != nullptr)
    {
        FGameplayEventData Data = *TriggerEventData;
        GameplayEventDataPtr.Emplace(Data);
    }

    PowerupComponent->OnCommitAbility(ActorInfo, ActivationInfo, TriggerEventData);
}
*/


void USGPowerupAbility::PreActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::PreActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    InitializeBase(ActorInfo, ActivationInfo, TriggerEventData);
}

void USGPowerupAbility::PostActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
}

void USGPowerupAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    PowerupComponent->OnEndAbility(ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool USGPowerupAbility::HandleGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData* Payload)
{
    if (Super::HandleGameplayEvent(EventTag, Payload))
    {
        return true;
    }
    
    if (EventTag == USGAbilitySystemGlobals::GetSG().Tag_Triggers_Ability_Shoot_Stop)
    {
        this->PauseAbility(this->CurrentSpecHandle, this->CurrentActorInfo, this->CurrentActivationInfo);
        return true;
    }
    

    return false;
}

void USGPowerupAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateCancelAbility)
{

    Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void USGPowerupAbility::PauseAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    if (PowerupComponent != nullptr)
    {
        PowerupComponent->ManualCancel();
    }
    
}

ASProjectileShooterCharacter* USGPowerupAbility::GetCharacter() const
{
    return Cast<ASProjectileShooterCharacter>(GetActorInfo().OwnerActor);
}

FGameplayTagContainer USGPowerupAbility::GetStopShootingTagContainer() const
{
    FGameplayTagContainer Container;
    Container.AddTag(GetStopShootingTag());
    return Container;
}

FGameplayTag USGPowerupAbility::GetStopShootingTag() const
{
    return USGAbilitySystemGlobals::GetSG().Tag_Triggers_Ability_Shoot_Stop;
}


