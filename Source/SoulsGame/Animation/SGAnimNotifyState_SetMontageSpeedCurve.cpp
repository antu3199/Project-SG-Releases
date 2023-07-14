// Fill out your copyright notice in the Description page of Project Settings.


#include "SGAnimNotifyState_SetMontageSpeedCurve.h"

#include "SoulsGame/SGUtils.h"

USGAnimNotifyState_SetMontageSpeedCurve::USGAnimNotifyState_SetMontageSpeedCurve() : Super()
{
}

void USGAnimNotifyState_SetMontageSpeedCurve::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	/*
	if (CachedTotalDuration != TotalDuration && TimelineComponent != nullptr)
	{
		TimelineComponent->SetPlayRate(1 / TotalDuration);
		CachedTotalDuration = TotalDuration;
	}
	
	if (bHasInitialized == false)
	{
		
		TimelineComponent = NewObject<UTimelineComponent>(MeshComp, FName("DashTimeline"));
		TimelineComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		TimelineComponent->SetNetAddressable();
		TimelineComponent->SetPropertySetObject(this);
		TimelineComponent->SetDirectionPropertyName(FName("TimelineDirection"));
		TimelineComponent->SetLooping(false);

		float MontageLength = TotalDuration;

		TimelineComponent->SetTimelineLength(1);
		TimelineComponent->SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);
		//DashTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
		TimelineComponent->SetPlayRate(1 / TotalDuration);


		FOnTimelineFloat OnTimelineCallback;
		OnTimelineCallback.BindDynamic(this, &UNS_SetMontageSpeedCurve::CurveTimelineCallback);
		TimelineComponent->AddInterpFloat(TimelineCurve, OnTimelineCallback, FName("Val"));

		FOnTimelineEventStatic OnTimelineFinishedCallback;
		OnTimelineFinishedCallback.BindUObject(this, &UNS_SetMontageSpeedCurve::CurveTimelineFinishedCallback);
		
		TimelineComponent->SetTimelineFinishedFunc(OnTimelineFinishedCallback);

		TimelineComponent->RegisterComponent();
		bHasInitialized = true;
	}
	*/

	
	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance null"));
		return;
	}

	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		UE_LOG(LogTemp, Warning, TEXT("NS_SetMontageSpeedCurve: RootMotion null"));
		return;
	}

	CachedRootMotion = RootMotion;
	BasePlayRate = RootMotion->GetPlayRate();
	
	
	if (ASGCharacterBase* CharacterBase = Cast<ASGCharacterBase>(GetCharacter(MeshComp)))
	{
		if (!CharacterBase->GetAllowSetMontageSpeed())
		{
			return;
		}
	}


	if (TimelineCurve != nullptr)
	{
		float PlayRate = TimelineCurve->GetFloatValue(0);

		if (ASGCharacterBase* CharacterBase = Cast<ASGCharacterBase>(GetCharacter(MeshComp)))
		{
			CharacterBase->SetCurrentMontageSpeed(PlayRate);
			PlayRate = CharacterBase->GetFinalMontageSpeed();
		}
	
		RootMotion->SetPlayRate(PlayRate);
	}


}

void USGAnimNotifyState_SetMontageSpeedCurve::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance null"));
		return;
	}

	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		UE_LOG(LogTemp, Warning, TEXT("NS_SetMontageSpeedCurve: RootMotion null"));
		return;
	}

	if (ASGCharacterBase* CharacterBase = Cast<ASGCharacterBase>(GetCharacter(MeshComp)))
	{
		if (!CharacterBase->GetAllowSetMontageSpeed())
		{
			return;
		}
	}
	
	
	/*
	if (TimelineComponent != nullptr)
	{
		TimelineComponent->Stop();
	}
	*/

	float Speed = BasePlayRate;

	if (ASGCharacterBase* CharacterBase = Cast<ASGCharacterBase>(GetCharacter(MeshComp)))
	{
		CharacterBase->SetCurrentMontageSpeed(BasePlayRate);
		Speed = CharacterBase->GetFinalMontageSpeed();
	}
	
	RootMotion->SetPlayRate(Speed);
}

void USGAnimNotifyState_SetMontageSpeedCurve::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (DisableMe)
	{
		return;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance null"));
		return;
	}

	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		UE_LOG(LogTemp, Warning, TEXT("NS_SetMontageSpeedCurve: RootMotion null"));
		return;
	}

	if (TimelineCurve == nullptr)
	{
		return;
	}

	if (ASGCharacterBase* CharacterBase = Cast<ASGCharacterBase>(GetCharacter(MeshComp)))
	{
		if (!CharacterBase->GetAllowSetMontageSpeed())
		{
			return;
		}
	}
	

	/*
	float PlayRate = TimelineCurve->GetFloatValue(CurrentTime / CachedTotalDuration);
	RootMotion->SetPlayRate(PlayRate);
	*/

	const FAnimNotifyEvent* Notify = AnimNotifyEventReference.GetNotify();
	float OffsetNotifyStart = Notify->GetTriggerTime();
	float CurrentPosition = RootMotion->GetPosition();
	float Duration = Notify->GetDuration();
	float Diff = CurrentPosition - OffsetNotifyStart;

	float t;
	if (bNormalizeCurve)
	{
		t = Diff / Duration;
	}
	else
	{
		t = Diff;
	}
	
	float PlayRate = TimelineCurve->GetFloatValue(t);
	if (ASGCharacterBase* CharacterBase = Cast<ASGCharacterBase>(GetCharacter(MeshComp)))
	{
		CharacterBase->SetCurrentMontageSpeed(PlayRate);
		PlayRate = CharacterBase->GetFinalMontageSpeed();
	}


	
	RootMotion->SetPlayRate(PlayRate);
}

