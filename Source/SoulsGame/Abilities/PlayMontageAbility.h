// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyGameplayAbility.h"
#include "PlayMontageAndWaitTask.h"
#include "Animation/AnimMontage.h"

#include "PlayMontageAbility.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMontageFinished, UMyGameplayAbility*);
/**
 * 
 */
UCLASS()
class SOULSGAME_API UPlayMontageAbility : public UMyGameplayAbility
{
	GENERATED_BODY()

public:
	bool GetEndOnMontageCompleted() const;
	UPlayMontageAndWaitTask * GetPlayMontageTask() const;
	FOnMontageFinished OnMontageFinished;

	virtual bool CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) override;

	virtual  void PreActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	
protected:
	// Callbacks (Must be a UPROPERTY() otherwise error)
	UFUNCTION()
    virtual void OnBlendOut(FGameplayTag GameplayTag, FGameplayEventData GameplayEventData);
	UFUNCTION()
    virtual void OnInterrupted(FGameplayTag GameplayTag, FGameplayEventData GameplayEventData);
	UFUNCTION()
    virtual void OnCancelled(FGameplayTag GameplayTag, FGameplayEventData GameplayEventData);
	UFUNCTION()
    virtual void OnCompleted(FGameplayTag GameplayTag, FGameplayEventData GameplayEventData);

	virtual bool HandleGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData* Payload) override;


	UFUNCTION(BlueprintNativeEvent)
	UAnimMontage* OverrideMontage();
	
	UPROPERTY(EditAnywhere)
	bool EndOnMontageCompleted = true;

	UPROPERTY(EditAnywhere)
	FPlayMontageAndWaitTaskData PlayMontageTaskData;

	UPROPERTY()
	UPlayMontageAndWaitTask * PlayMontageTask;

	UPROPERTY(EditAnywhere)
	bool PlayMontage = true;


	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void EndAbility(const bool WasCancelled = false) override;

	
};


