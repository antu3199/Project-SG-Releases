// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyAnimNotifyState.h"
#include "NS_SetMontageSpeed.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API UNS_SetMontageSpeed : public UMyAnimNotifyState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float SetPlayRate = 1.0f;

	UPROPERTY(EditAnywhere)
	float TimestoppedPlayRate = 1.0f;

	UPROPERTY(EditAnywhere)
	bool bUseTimestoppedPlayRate = false;

	UPROPERTY(EditAnywhere)
	bool bDebug_UseTimestoppedRate = false;
	
	
	void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	float BasePlayRate = 1.0f;

};
