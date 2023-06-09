// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_SlowDownAnimationIfEnemy.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGAnimNotifyState_SlowDownAnimationIfEnemy : public USGAnimNotifyState
{
	GENERATED_BODY()

protected:
	void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere)
	bool DebugAnimation = false;


	UPROPERTY(EditAnywhere)
	float SetAnimationRateMin = 0.1f;

	UPROPERTY(EditAnywhere)
	float SetAnimationRateMax = 0.4f;
	
	float BasePlayRate = 1.0f;
};
