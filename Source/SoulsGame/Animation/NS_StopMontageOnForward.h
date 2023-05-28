// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAnimNotifyState.h"
#include "NS_StopMontageOnForward.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API UNS_StopMontageOnForward : public UMyAnimNotifyState
{
	GENERATED_BODY()
public:

protected:
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;


	UPROPERTY(EditAnywhere)
	float ForwardDistance = 40.0f;
	
};
