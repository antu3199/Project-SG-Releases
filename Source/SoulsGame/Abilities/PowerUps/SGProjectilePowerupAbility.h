
#pragma once
#include "CoreMinimal.h"
#include "SGPowerupAbility.h"
#include "SGProjectilePowerupAbility.generated.h"


USTRUCT()
struct FSGShootProjectileParms
{
    GENERATED_BODY()

    TWeakObjectPtr<class USGProjectilePowerupComponent> ShotComponent = nullptr;
};


UCLASS(Blueprintable)
class SOULSGAME_API USGProjectilePowerupAbility : public USGPowerupAbility
{
    GENERATED_BODY()
public:

protected:
    
    virtual void PreActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	
    virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;


    virtual void OnShotFinished();

    virtual void OnShootingFinished();

    virtual void OnPreShotHandle();
    FGameplayTagContainer GetStopShootingTagContainer() const;
    FGameplayTag GetStopShootingTag() const;

    bool bIsPlayerCharacter = false;

    UPROPERTY()
    class ASCharacterBase * ShootCharacter = nullptr;


    UPROPERTY(EditAnywhere)
    FSGShootProjectileParms ProjectileParms;

    int32 ShotsFinished = 0;
};
