// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_OverrideRotation.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGAnimNotifyState_OverrideRotation : public USGAnimNotifyState
{
	GENERATED_BODY()

protected:
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

};
