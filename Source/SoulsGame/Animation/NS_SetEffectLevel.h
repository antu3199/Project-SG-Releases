// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyAnimNotifyState.h"
#include "NS_SetEffectLevel.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API UNS_SetEffectLevel : public UMyAnimNotifyState
{
	GENERATED_BODY()
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	UPROPERTY(EditAnywhere)
	int32 LevelEffectIndex = -1;

	UPROPERTY(EditAnywhere)
	int32 LevelEffectLevel = 0;
	
	
};
