#include "SGAbility_PlayMontageAndWait_Task.h"
#include "SGAbility.h"
#include "SGAbilityComponent.h"
#include "GameFramework/Character.h"

USGAbility_PlayMontageAndWait_Task::USGAbility_PlayMontageAndWait_Task(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Rate = 1.f;
	bStopWhenAbilityEnds = true;
}

void USGAbility_PlayMontageAndWait_Task::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
	{
		if (Montage == MontageToPlay)
		{
			Ability->SetCurrentMontage(nullptr);

			// Reset AnimRootMotionTranslationScale
			ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
			if (Character)
			{
				Character->SetAnimRootMotionTranslationScale(1.f);
			}

		}
	}

	if (bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast();
			OnMontageEndForAnyReason.Broadcast();
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnBlendOut.Broadcast();
			OnMontageEndForAnyReason.Broadcast();
		}
	}
}

void USGAbility_PlayMontageAndWait_Task::OnMontageInterrupted()
{
	if (StopPlayingMontage())
	{
		// Let the BP handle the interrupt as well
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast();
			OnMontageEndForAnyReason.Broadcast();
		}
	}
}

void USGAbility_PlayMontageAndWait_Task::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast();
			OnMontageEndForAnyReason.Broadcast();
		}
	}

	EndTask();
}

USGAbility_PlayMontageAndWait_Task* USGAbility_PlayMontageAndWait_Task::CreatePlayMontageAndWaitProxy(USGAbility* OwningAbility, FName TaskInstanceName,
	UAnimMontage* MontageToPlay, float InRate, FName StartSection, bool bStopWhenAbilityEnds,
	float AnimRootMotionTranslationScale, float InStartTimeSeconds)
{
	USGAbility_PlayMontageAndWait_Task* MyObj = NewSGAbilityTask<USGAbility_PlayMontageAndWait_Task>(OwningAbility, TaskInstanceName);
	MyObj->MontageToPlay = MontageToPlay;
	MyObj->Rate = InRate;
	MyObj->StartSection = StartSection;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	MyObj->StartTimeSeconds = InStartTimeSeconds;
	
	return MyObj;
}

void USGAbility_PlayMontageAndWait_Task::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}

	bool bPlayedMontage = false;

	if (AbilityComponent)
	{
		const FSGAbilityActorInfo ActorInfo = Ability->GetActorInfo();
		UAnimInstance* AnimInstance = ActorInfo.GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			float Duration = AnimInstance->Montage_Play(MontageToPlay, Rate, EMontagePlayReturnType::MontageLength, StartTimeSeconds);
			Ability->SetCurrentMontage(MontageToPlay);
			if (StartSection != NAME_None)
			{
				AnimInstance->Montage_JumpToSection(StartSection, MontageToPlay);
			}

			// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
			if (ShouldBroadcastAbilityTaskDelegates() == false)
			{
				UE_LOG(LogTemp, Warning, TEXT("PlayMontageAndWait not done! %s"), *GetNameSafe(Ability));
				return;
			}

			Ability->OnAbilityEnd.AddDynamic(this, &USGAbility_PlayMontageAndWait_Task::OnAbilityEnded);

			BlendingOutDelegate.BindUObject(this, &USGAbility_PlayMontageAndWait_Task::OnMontageBlendingOut);
			AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

			MontageEndedDelegate.BindUObject(this, &USGAbility_PlayMontageAndWait_Task::OnMontageEnded);
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

			ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
			if (Character)
			{
				Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
			}

			bPlayedMontage = true;
		}
		else
		{
			ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWait call to PlayMontage failed!"));
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWait called on invalid AbilitySystemComponent"));
	}

	if (!bPlayedMontage)
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWait called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay),*InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast();
			OnMontageEndForAnyReason.Broadcast();
		}
	}
}

void USGAbility_PlayMontageAndWait_Task::ExternalCancel()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCancelled.Broadcast();
		OnMontageEndForAnyReason.Broadcast();
	}
	Super::ExternalCancel();
}

FString USGAbility_PlayMontageAndWait_Task::GetDebugString() const
{
	UAnimMontage* PlayingMontage = nullptr;
	if (Ability)
	{
		const FSGAbilityActorInfo& ActorInfo = Ability->GetActorInfo();
		UAnimInstance* AnimInstance = ActorInfo.GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay : AnimInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(TEXT("PlayMontageAndWait. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}

void USGAbility_PlayMontageAndWait_Task::OnAbilityEnded()
{
	if (StopPlayingMontage())
	{
		// Let the BP handle the interrupt as well
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast();
			OnMontageEndForAnyReason.Broadcast();
		}
	}
}

void USGAbility_PlayMontageAndWait_Task::OnDestroy(bool AbilityEnded)
{
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)

	// This delegate, however, should be cleared as it is a multicast
	if (Ability)
	{
		Ability->OnAbilityEnd.RemoveAll(this);
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}

	Super::OnDestroy(AbilityEnded);
}

bool USGAbility_PlayMontageAndWait_Task::StopPlayingMontage()
{
	const FSGAbilityActorInfo& ActorInfo = Ability->GetActorInfo();
	UAnimInstance* AnimInstance = ActorInfo.GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return false;
	}

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
	if (AbilityComponent && Ability)
	{

		// Unbind delegates so they don't get called as well
		FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
		if (MontageInstance)
		{
			MontageInstance->OnMontageBlendingOutStarted.Unbind();
			MontageInstance->OnMontageEnded.Unbind();
		}
		bool bShouldStopMontage = AnimInstance && MontageToPlay && !AnimInstance->Montage_GetIsStopped(MontageToPlay);
		if (bShouldStopMontage)
		{
			const float BlendOutTime = MontageToPlay->BlendOut.GetBlendTime();

			AnimInstance->Montage_Stop(BlendOutTime, MontageToPlay);
		}
		return true;
	}

	return false;
}
