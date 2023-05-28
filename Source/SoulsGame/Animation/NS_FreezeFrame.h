// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyAnimNotifyState.h"
#include "NS_FreezeFrame.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API UNS_FreezeFrame : public UMyAnimNotifyState
{
	GENERATED_BODY()
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	void ResetAnim();

	void OnFreezeFrameConditionMet();

	UPROPERTY(EditAnywhere)
	float FrozenMontageSpeed = 0.0001f;
	
	UPROPERTY(EditAnywhere)
	float FreezeFrameTime = 3.0f;

	UPROPERTY(EditAnywhere)
	bool bListenToFreezeFrameCondition = false;
	
	float BasePlayRate = 1.0f;

	UPROPERTY(Transient)
	AActor* CachedActor = nullptr;

	UPROPERTY(Transient)
	USkeletalMeshComponent* CachedMeshComp = nullptr;

	FTimerHandle FreezeTimerHandle;

	bool bActivated = false;
	
};


