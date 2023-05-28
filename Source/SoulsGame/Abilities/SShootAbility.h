
#pragma once
#include "CoreMinimal.h"
#include "MyGameplayAbility.h"

#include "SShootAbility.generated.h"

USTRUCT()
struct FShootProjectileParms
{
    GENERATED_BODY()
    
    FDelegateHandle ShotFinishedHandle;
    FDelegateHandle ShootingFinishedHandle;
    FDelegateHandle PreShotHandle;
    UPROPERTY()
    class UProjectileShooterComponent * ShotComponent;
};

UCLASS()
class SOULSGAME_API USShootAbility: public UMyGameplayAbility
{
    GENERATED_BODY()
public:
    virtual void SetLevel(int32 Level) override;

    virtual void DoActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    
protected:
    
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	
    virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

    virtual void InitializeEffectContainerHelper() override;

    virtual bool HandleGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData* Payload) override;

    class ASProjectileShooterCharacter * GetCharacter() const;

    virtual void OnShotFinished();

    virtual void OnShootingFinished();

    virtual void OnPreShotHandle();

    FVector GetDefaultTarget() const;
    
    UPROPERTY(EditAnywhere)
    TArray<FString> ProjectileSpawnerNames = { TEXT("PShooter_SingleShot") };


    bool bIsPlayerCharacter = false;

    UPROPERTY()
    class ASCharacterBase * ShootCharacter = nullptr;
    FGameplayTagContainer GetStopShootingTagContainer() const;
    FGameplayTag GetStopShootingTag() const;

    UPROPERTY()
    TArray<FShootProjectileParms> ProjectileParms;

    int32 ShotsFinished = 0;

};
