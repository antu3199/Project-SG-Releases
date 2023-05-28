// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAnimNotifyState.h"
#include "NS_DisableInputForNonRootMotion.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API UNS_DisableInputForNonRootMotion : public UMyAnimNotifyState
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool DisableInputForNonRootMotionStart = true;

	UPROPERTY(EditAnywhere)
	bool DisableInputForNonRootMotionEnd = false;


protected:
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

};
