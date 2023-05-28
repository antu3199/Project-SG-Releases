#include "SShootAbility.h"


#include "GameplayTagsManager.h"
#include "SGAbilitySystemGlobals.h"
#include "Kismet/GameplayStatics.h"
#include "SoulsGame/Character/SHumanoidPlayerCharacter.h"
#include "SoulsGame/Character/SProjectileShooterCharacter.h"

void USShootAbility::SetLevel(int32 Level)
{
    Super::SetLevel(Level);

    for (auto& Projectiles : ProjectileParms)
    {
        if (!Projectiles.ShotComponent)
        {
            continue;
        }
        
        Projectiles.ShotComponent->SetLevel(Level);
    }
}

void USShootAbility::DoActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
    Super::DoActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    ShootCharacter = Cast<ASCharacterBase>(GetActorInfo().OwnerActor);

    if (ShootCharacter == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("ShooterCharacter was null"));
        return;
    }
    
    
    bIsPlayerCharacter = ShootCharacter->IsPlayerControlled();
    FVector DefaultTarget = GetDefaultTarget();

    ProjectileParms.Reset();

    TArray<UProjectileShooterComponent*> ShooterComponents;
    ShootCharacter->GetComponents<UProjectileShooterComponent>(ShooterComponents);
    
    for (auto& ProjectileName : ProjectileSpawnerNames)
    {
        /*
        UProjectileShooterComponent * Component = ShootCharacter->GetProjectileSpawner(ProjectileName);
        if (Component == nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("Cannot find component: %s"), *ProjectileName);
            continue;
        }

        FShootProjectileParms ShootParms;
        ShootParms.ShotComponent = Component;
        ShootParms.ShotFinishedHandle = Component->OnShotFinished.AddUObject(this, &USShootAbility::OnShotFinished);
        ShootParms.ShootingFinishedHandle = Component->OnShootingFinished.AddUObject(this, &USShootAbility::OnShootingFinished);
        ShootParms.PreShotHandle = Component->OnPreShot.AddUObject(this, &USShootAbility::OnPreShotHandle);

        // TODO: Refactor this
        // ShootCharacter->SetTargetVector(Component, DefaultTarget);
        // ShootCharacter->StartShooting(Component);

        Component->SetLevel(CurrentLevel);
        
        ProjectileParms.Add(ShootParms);
        */
    }

    ShotsFinished = 0;

    // TODO: Refactor this
    // ShootCharacter->ApplyEffectsToAbility(this);
}

void USShootAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    for (auto & ShootParms : ProjectileParms)
    {
        if (ShootParms.ShotFinishedHandle.IsValid())
        {
            ShootParms.ShotComponent->OnShotFinished.Remove(ShootParms.ShotFinishedHandle);
            ShootParms.ShotFinishedHandle.Reset();
        }
    
        if (ShootParms.ShootingFinishedHandle.IsValid())
        {
            ShootParms.ShotComponent->OnShootingFinished.Remove(ShootParms.ShootingFinishedHandle);
            ShootParms.ShootingFinishedHandle.Reset();
        }

        if (ShootParms.PreShotHandle.IsValid())
        {
            ShootParms.ShotComponent->OnPreShot.Remove(ShootParms.PreShotHandle);
            ShootParms.PreShotHandle.Reset();
        }
    }
    
    ASHumanoidPlayerCharacter* PlayerCharacter =  Cast<ASHumanoidPlayerCharacter>(GetActorInfo().OwnerActor);
    
    if (PlayerCharacter != nullptr && PlayerCharacter->GetAbilitySystemComponent())
    {
        PlayerCharacter->GetAbilitySystemComponent()->RemoveGameplayEventTagContainerDelegate(GetStopShootingTagContainer(), EventDelegateHandle);
    }

    EventDelegateHandle.Reset();
}

bool USShootAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    FGameplayTagContainer* OptionalRelevantTags) const
{
    return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
}

void USShootAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo) const
{
    Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}

void USShootAbility::InitializeEffectContainerHelper()
{
    Super::InitializeEffectContainerHelper();
}


bool USShootAbility::HandleGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData* Payload)
{
    if (EventTag == USGAbilitySystemGlobals::GetSG().Tag_Triggers_Ability_Shoot_Stop)
    {
        for (auto & ShootParms : ProjectileParms)
        {
            // TODO: Refactor this
            // ShootCharacter->StopShooting(ShootParms.ShotComponent);
        }
    }
    
    return Super::HandleGameplayEvent(EventTag, Payload);
}

ASProjectileShooterCharacter* USShootAbility::GetCharacter() const
{
    return Cast<ASProjectileShooterCharacter>(GetActorInfo().OwnerActor);
}

void USShootAbility::OnShotFinished()
{
}

void USShootAbility::OnShootingFinished()
{
    ShotsFinished++;
    if (ShotsFinished >= ProjectileSpawnerNames.Num())
    {
        this->EndAbility(this->CurrentSpecHandle, this->CurrentActorInfo, this->CurrentActivationInfo, true, false);
    }
}

void USShootAbility::OnPreShotHandle()
{

    /*
    USceneComponent* DefaultHomingTarget = ShootCharacter->GetDefaultHomingTarget();

    for (auto & ShootParms : ProjectileParms)
    {
        if (DefaultHomingTarget != nullptr)
        {
            ShootCharacter->SetHomingTarget(ShootParms.ShotComponent, DefaultHomingTarget);
        }
        if (bIsPlayerCharacter)
        {
            FVector DefaultTarget = ShootCharacter->GetDefaultTarget();
            ShootCharacter->SetTargetVector(ShootParms.ShotComponent, DefaultTarget);
        }
    }
    */
            
}

FVector USShootAbility::GetDefaultTarget() const
{
    if (ASProjectileShooterCharacter* ShooterCharacter = Cast<ASProjectileShooterCharacter>(GetActorInfo().OwnerActor))
    {
        return ShooterCharacter->GetDefaultTarget();
    }

    if (ASHumanoidPlayerCharacter* HumanoidPlayerCharacter = Cast<ASHumanoidPlayerCharacter>(GetActorInfo().OwnerActor))
    {
        return HumanoidPlayerCharacter->GetTargetLocation();
        /*
        if (AConfectioniteCharacter* ConfectioniteCharacter = Cast<AConfectioniteCharacter>(HumanoidPlayerCharacter->GetConfectioniteCharacter()))
        {
            return ConfectioniteCharacter->GetTargetLocation();
        }
        */
    }

    return FVector::ZeroVector;
}

FGameplayTagContainer USShootAbility::GetStopShootingTagContainer() const
{
    FGameplayTagContainer Container;
    Container.AddTag(GetStopShootingTag());
    return Container;
}

FGameplayTag USShootAbility::GetStopShootingTag() const
{
    return USGAbilitySystemGlobals::GetSG().Tag_Triggers_Ability_Shoot_Stop;
}

