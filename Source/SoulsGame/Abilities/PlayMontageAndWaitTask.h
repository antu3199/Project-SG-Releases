// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SoulsGame/Abilities/CharacterAbilitySystemComponent.h"

#include "PlayMontageAndWaitTask.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayMontageAndWaitTaskDelegate, FGameplayTag, varNameFGameplayTag, FGameplayEventData, varNameFGameplayEventData);

USTRUCT(BlueprintType)
struct FPlayMontageTaskData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UAnimMontage *MontageToPlay;
	
	UPROPERTY(EditAnywhere)
	FName TaskInstanceName = "TaskName";
	UPROPERTY(EditAnywhere)
	float Rate = 1.0;
	UPROPERTY(EditAnywhere)
	FName StartSection = "None";
	UPROPERTY(EditAnywhere)
	bool StopWhenAbilityEnds = true;
	UPROPERTY(EditAnywhere)
	float AnimRootMotionTranslationScale = 0.01f; // Houdini conversion
	
	UPROPERTY()
	UGameplayAbility* OwningAbility = nullptr;
};

USTRUCT(BlueprintType)
struct FPlayMontageAndWaitTaskData : public FPlayMontageTaskData
{
	GENERATED_BODY()
};

/**
 * 
 */
UCLASS()
class SOULSGAME_API UPlayMontageAndWaitTask : public UAbilityTask
{
	GENERATED_BODY()


public:
	UPlayMontageAndWaitTask(const FObjectInitializer & ObjectInitializer);

	// Callbacks
	FPlayMontageAndWaitTaskDelegate OnCompleted;
	FPlayMontageAndWaitTaskDelegate OnBlendOut;
	FPlayMontageAndWaitTaskDelegate OnInterrupted;
	FPlayMontageAndWaitTaskDelegate OnCancelled;


	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UPlayMontageAndWaitTask *CreatePlayMontageAndWaitEvent(FPlayMontageAndWaitTaskData & TaskData);

protected:
	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	FPlayMontageAndWaitTaskData TaskData;
	UCharacterAbilitySystemComponent * GetTargetAbilitySystemComponent() const;

	// Delegate handles
	FDelegateHandle CancelledDelegateHandle;
	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;

	// Delegate callbacks
	bool StopPlayingMontage();
	void OnAbilityCancelled();
	void OnMontageEnded(UAnimMontage  * Montage, bool bInterrupted);
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	bool PlayedMontage = false;
};
