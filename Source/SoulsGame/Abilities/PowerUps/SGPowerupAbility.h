
#pragma once
#include "CoreMinimal.h"
#include "SoulsGame/Abilities/MyGameplayAbility.h"
#include "SoulsGame/Abilities/PowerUps/Components/SGPowerupComponent.h"

#include "SGPowerupAbility.generated.h"



UCLASS()
class SOULSGAME_API USGPowerupAbility: public UMyGameplayAbility
{
    GENERATED_BODY()
public:
    virtual void SetLevel(int32 Level) override;
    virtual USGPowerupComponent* GetPowerupComponent() const;
    virtual const TOptional<FGameplayEventData>& GetGameplayEventData() const;
    
    virtual void DoActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual  void PreActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual  void PostActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    
protected:
    virtual void InitializeBase(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo,
                                        const FGameplayEventData* TriggerEventData);

    //virtual  void OnCommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


    virtual bool HandleGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData* Payload) override;

    virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

    virtual void PauseAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);


    
    
    class ASProjectileShooterCharacter * GetCharacter() const;

    FGameplayTagContainer GetStopShootingTagContainer() const;
    FGameplayTag GetStopShootingTag() const;

    
    UPROPERTY(EditAnywhere)
    TSubclassOf<USGPowerupComponent> PowerupComponentTemplate = nullptr;

    UPROPERTY(EditAnywhere)
    bool bAutoActivateComponent = true;
    
    TOptional<FGameplayEventData> GameplayEventDataPtr;

    UPROPERTY(Transient, BlueprintReadOnly)
    USGPowerupComponent* PowerupComponent = nullptr;

    UPROPERTY(Transient, BlueprintReadOnly)
    AActor* ComponentOwner = nullptr;

    
};
