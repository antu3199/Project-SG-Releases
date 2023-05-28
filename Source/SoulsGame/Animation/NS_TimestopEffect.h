// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyAnimNotifyState.h"
#include "NS_TimestopEffect.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API UNS_TimestopEffect : public UMyAnimNotifyState
{
	GENERATED_BODY()
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;
};
