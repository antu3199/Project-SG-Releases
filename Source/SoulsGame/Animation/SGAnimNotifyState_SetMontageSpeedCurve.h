// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_SetMontageSpeedCurve.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGAnimNotifyState_SetMontageSpeedCurve : public USGAnimNotifyState
{
	GENERATED_BODY()

	USGAnimNotifyState_SetMontageSpeedCurve();

	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override; 

	UPROPERTY(EditAnywhere)
	UCurveFloat* TimelineCurve;

	UPROPERTY(EditAnywhere)
	bool bNormalizeCurve;
	
	//UPROPERTY(EditAnywhere, Category = "Curve Category")
	//FRuntimeFloatCurve TestCurve;
	// Better curve, but worse performance

	float BasePlayRate = 1.0f;

	FAnimMontageInstance* CachedRootMotion = nullptr;
};
