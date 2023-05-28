// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyAnimNotifyState.h"
#include "NS_RotateActor.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API UNS_RotateActor : public UMyAnimNotifyState
{
	GENERATED_BODY()

	UNS_RotateActor();

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
