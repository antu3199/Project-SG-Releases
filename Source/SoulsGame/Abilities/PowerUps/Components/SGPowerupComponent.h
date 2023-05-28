#pragma once
#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "GameplayPrediction.h"
#include "SGPowerupStats.h"

#include "SGPowerupComponent.generated.h"


struct FGameplayEventData;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSGOnPowerupFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSGOnPreDoPowerup);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSGOnPostDoPowerup);

UCLASS(Abstract, Blueprintable)
class USGPowerupComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	USGPowerupComponent();

	virtual void OnInitializeAbilityContext(class USGPowerupAbility* Ability, AActor* InAbilitySystemOwner, AActor* InComponentOwner);
	virtual void OnInitializeOneTime();
	
	//virtual void OnCommitAbility(const struct FGameplayAbilityActorInfo* ActorInfo, const struct FGameplayAbilityActivationInfo& ActivationInfo,
	//const struct FGameplayEventData* TriggerEventData);
	virtual void OnActivateAbility(const struct FGameplayAbilityActorInfo* ActorInfo, const struct FGameplayAbilityActivationInfo& ActivationInfo, const struct FGameplayEventData* TriggerEventData);
	virtual void OnEndAbility(const FGameplayAbilityActorInfo* ActorInfo, const struct FGameplayAbilityActivationInfo& ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);
	
	virtual void ManualActivateAbility();


	UFUNCTION(BlueprintCallable)
	virtual void ManualCancel();
	
	bool GetDetachOnEndAbility() const;

	UFUNCTION(BlueprintCallable)
	int32 GetLevel();

	UFUNCTION(BlueprintCallable)
	void SetLevel(int32 Level);

	virtual void GetLifetimeReplicatedProps(::TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool HasInitialized() const;

	void SetTarget(USceneComponent * InTarget);

	UFUNCTION(BlueprintCallable)
	void SetTargetVector(FVector InTarget);


	UPROPERTY(BlueprintAssignable)
	FSGOnPreDoPowerup OnPreDoPowerup;
	
	UPROPERTY(BlueprintAssignable)
	FSGOnPostDoPowerup OnPostDoPowerup;

	UPROPERTY(BlueprintAssignable)
	FSGOnPowerupFinished OnPowerupFinished;

	UPROPERTY(EditAnywhere)
	bool bStartPowerupOnActivated = false;

	UFUNCTION(BlueprintCallable)
	virtual void StartPowerup();
	
	UFUNCTION(BlueprintCallable)
	virtual void StopPowerup();

protected:

	virtual void RequestPowerup();
	virtual void DoPowerup();

	virtual void SetNextShotAsLast();

	virtual bool CanDoPowerup() const;
	virtual void PreShot();
	virtual void PostShot();

	FVector GetRelativeCharacterForward() const;
	FVector GetRelativeCharacterRight() const;
	FVector GetRelativeCharacterUp() const;

	FVector GetCameraRelativeVector(const FVector& Direction) const;
	FVector GetCameraRelativeRight() const;
	FVector GetCameraRelativeUp() const;


	virtual void OnTargetUpdated(const FVector& Direction);
	
	UPROPERTY(EditAnywhere)
	bool bDetachOnEndAbility = false;

	// Note: Transient flag needed otherwise it's garbage collected. Not sure why...
	UPROPERTY(Replicated)
	FSGPowerupStats Stats;

	UPROPERTY(EditAnywhere)
	bool bPlayLooping = true;

	UPROPERTY(EditAnywhere)
	FName ProjectileSceneComponentName = "SGProjectilePowerup_SceneComponent0";

	
	bool bIsActivated = false;

	TWeakObjectPtr<AActor> AbilitySystemOwner = nullptr;
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;
	TWeakObjectPtr<AActor> ComponentOwner = nullptr;
	
	TWeakObjectPtr<USGPowerupAbility> PowerupAbility = nullptr;
	TWeakPtr<struct FGameplayEffectsWrapperContainer> GameplayEffectsContainer = nullptr;

	int32 CurrentLevel = 0;

	bool bInitialized = false;

	FTimerHandle ShootingHandle;
	FTimerHandle LoopHandle;
	
	UPROPERTY(EditAnywhere)
	float LoopDelay = 1;

	UPROPERTY(EditAnywhere)
	float LoopForSeconds = 3;
	
	bool bNextShotAsLast = false;

	bool bIsShooting = false;
	double LastShotTime = 0;

	UPROPERTY()
	USceneComponent * Target = nullptr;

	FVector TargetVector = FVector::ZeroVector;
	bool bUseTargetVector;

	bool bDoPostPowerupImmediately = true;


	// Ability context. Not sure if want to keep this.
	//const FGameplayAbilityActorInfo* ActorInfo = nullptr;
	//FGameplayAbilityActivationInfo ActivationInfo;
	//const FGameplayEventData* TriggerEventData = nullptr;
	

};
