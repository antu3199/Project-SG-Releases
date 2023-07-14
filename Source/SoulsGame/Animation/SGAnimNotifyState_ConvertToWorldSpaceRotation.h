// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_ConvertToWorldSpaceRotation.generated.h"

/**
 * Note: MAKE SURE THAT BLEND TIME is equal to 0 before using this.
 */
UCLASS()
class SOULSGAME_API USGAnimNotifyState_ConvertToWorldSpaceRotation : public USGAnimNotifyState
{
	GENERATED_BODY()

protected:
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	virtual  void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	FTransform GetBoneTransFromMontage(UAnimMontage* InMontage, FName BoneName, float InTime);
	
	FTransform PreviousTransform;
	FTransform InitialParentTransform;

	FVector InitialActorLocation;
	FVector InitialBoneLocation;
	
	FRotator InitialRelativeRotation;

	FQuat GetRotationFromAnimation(const FTransform& Transform) const;

	void DoRotation(USkeletalMeshComponent* MeshComp, float TestTime = -1);

	
	UAnimMontage * ActiveMontage;

	// Note: Disabling additive is more accurate, but does not working with blending
	UPROPERTY(EditAnywhere)
	bool AdditiveAnimation = true;

};
