// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyAnimNotifyState.h"
#include "NS_ResetPreviewActor.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API UNS_ResetPreviewActor : public UMyAnimNotifyState
{
	GENERATED_BODY()

	UNS_ResetPreviewActor();
	
	UPROPERTY(EditAnywhere)
	FVector InitialRotation;

	virtual void DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	bool bAbleToTrigger = true;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere)
	bool bResetInGameVelocityAsWell = true;
};
