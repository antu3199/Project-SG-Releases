// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyAnimNotifyState.h"
#include "NS_LoopAnim.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API UNS_LoopAnim : public UMyAnimNotifyState
{
	GENERATED_BODY()
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	UPROPERTY(EditAnywhere)
	int32 NumLoops = -1;

	int32 CurNumLoops = 0;
	
};
