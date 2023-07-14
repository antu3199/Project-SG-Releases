// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_RotateActor.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGAnimNotifyState_RotateActor : public USGAnimNotifyState
{
	GENERATED_BODY()

	USGAnimNotifyState_RotateActor();

	virtual void DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override; 

	UPROPERTY(EditAnywhere)
	UCurveVector* RotationCurve;
	
	UPROPERTY(EditAnywhere)
	FVector DefaultPreviewRotation = FVector(0, 0, -90);
	
	UPROPERTY(Transient)
	AActor* CachedActor = nullptr;

	FQuat InitialRotation;
	
};
