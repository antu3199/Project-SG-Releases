// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_AllowInputDuringMontage.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGAnimNotifyState_AllowInputDuringMontage : public USGAnimNotifyState
{
	GENERATED_BODY()
protected:
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	UPROPERTY(EditAnywhere)
	bool CancelMontage = true;
	
};
