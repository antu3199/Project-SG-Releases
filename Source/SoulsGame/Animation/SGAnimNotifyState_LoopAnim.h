// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_LoopAnim.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGAnimNotifyState_LoopAnim : public USGAnimNotifyState
{
	GENERATED_BODY()
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	UPROPERTY(EditAnywhere)
	int32 NumLoops = -1;

	int32 CurNumLoops = 0;
	
};
