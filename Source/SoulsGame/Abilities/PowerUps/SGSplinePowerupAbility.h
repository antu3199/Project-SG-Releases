
#pragma once
#include "CoreMinimal.h"
#include "SGPowerupAbility.h"
#include "SGSplinePowerupAbility.generated.h"


USTRUCT()
struct FSGSplineAbilityParams
{
    GENERATED_BODY()
    
   //FDelegateHandle ShotFinishedHandle;
   //FDelegateHandle ShootingFinishedHandle;
   //FDelegateHandle PreShotHandle;
    TWeakObjectPtr<class USGSpawnSplineActorPowerupComponent> SplineComponent = nullptr;
};


UCLASS(Blueprintable)
class SOULSGAME_API USGSplinePowerupAbility : public USGPowerupAbility
{
    GENERATED_BODY()
public:

protected:
    
    virtual void PreActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    void OnSplineFinished();


    UPROPERTY()
    class ASCharacterBase * BaseCharacter = nullptr;

    UPROPERTY(EditAnywhere)
    FSGSplineAbilityParams AbilityParms;
};
