// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_ResetPreviewActor.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGAnimNotifyState_ResetPreviewActor : public USGAnimNotifyState
{
	GENERATED_BODY()

	USGAnimNotifyState_ResetPreviewActor();
	
	UPROPERTY(EditAnywhere)
	FVector InitialRotation;

	virtual void DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	bool bAbleToTrigger = true;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere)
	bool bResetInGameVelocityAsWell = true;
};
