#pragma once

#include "CoreMinimal.h"
#include "SGAbilityTask.h"
#include "SGAbility_PlayMontageAndWait_Task.generated.h"

// Similar to AbilityTask_PlayMontageAndWait, but without dependence on abiltiy system
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSGMontageWaitSimpleDelegate);

UCLASS()
class SOULSGAME_API USGAbility_PlayMontageAndWait_Task : public USGAbilityTask
{
	GENERATED_BODY()

public:
	USGAbility_PlayMontageAndWait_Task(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(BlueprintAssignable)
	FSGMontageWaitSimpleDelegate OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FSGMontageWaitSimpleDelegate OnBlendOut;

	UPROPERTY(BlueprintAssignable)
	FSGMontageWaitSimpleDelegate OnInterrupted;

	UPROPERTY(BlueprintAssignable)
	FSGMontageWaitSimpleDelegate OnCancelled;

	UPROPERTY(BlueprintAssignable)
	FSGMontageWaitSimpleDelegate OnMontageEndForAnyReason;
	
	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	/** 
	 * Start playing an animation montage on the avatar actor and wait for it to finish
	 * If StopWhenAbilityEnds is true, this montage will be aborted if the ability ends normally. It is always stopped when the ability is explicitly cancelled.
	 * On normal execution, OnBlendOut is called when the montage is blending out, and OnCompleted when it is completely done playing
	 * OnInterrupted is called if another montage overwrites this, and OnCancelled is called if the ability or task is cancelled
	 *
	 * @param TaskInstanceName Set to override the name of this task, for later querying
	 * @param MontageToPlay The montage to play on the character
	 * @param InRate Change to play the montage faster or slower
	 * @param InStartSection If not empty, named montage section to start from
	 * @param InbStopWhenAbilityEnds If true, this montage will be aborted if the ability ends normally. It is always stopped when the ability is explicitly cancelled
	 * @param InAnimRootMotionTranslationScale Change to modify size of root motion or set to 0 to block it entirely
	 * @param InStartTimeSeconds Starting time offset in montage, this will be overridden by StartSection if that is also set
	 */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (DisplayName="SG Ability PlayMontageAndWait",
		HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static USGAbility_PlayMontageAndWait_Task* CreatePlayMontageAndWaitProxy(class USGAbility* OwningAbility, FName TaskInstanceName, UAnimMontage* MontageToPlay, float Rate = 1.f, FName StartSection = NAME_None, bool bStopWhenAbilityEnds = true, float AnimRootMotionTranslationScale = 1.f, float StartTimeSeconds = 0.f);

	virtual void Activate() override;

	/** Called when the ability is asked to cancel from an outside node. What this means depends on the individual task. By default, this does nothing other than ending the task. */
	virtual void ExternalCancel() override;

	virtual FString GetDebugString() const override;

protected:

	UFUNCTION()
	void OnAbilityEnded();
	
	virtual void OnDestroy(bool AbilityEnded) override;

	/** Checks if the ability is playing a montage and stops that montage, returns true if a montage was stopped, false if not. */
	bool StopPlayingMontage();

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;

	UPROPERTY()
	UAnimMontage* MontageToPlay;

	UPROPERTY()
	float Rate;

	UPROPERTY()
	FName StartSection;

	UPROPERTY()
	float AnimRootMotionTranslationScale;

	UPROPERTY()
	float StartTimeSeconds;

	UPROPERTY()
	bool bStopWhenAbilityEnds;
};
