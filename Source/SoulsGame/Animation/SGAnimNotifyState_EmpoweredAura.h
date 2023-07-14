// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_EmpoweredAura.generated.h"

/**
 * 
 */


UCLASS()
class SOULSGAME_API USGAnimNotifyState_EmpoweredAura : public USGAnimNotifyState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> FXTemplate;

	UPROPERTY(EditAnywhere)
	FVector OffsetVector = FVector::ZeroVector;
	
	
protected:
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;
};
