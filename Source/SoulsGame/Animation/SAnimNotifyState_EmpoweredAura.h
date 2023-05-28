// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAnimNotifyState.h"
#include "SAnimNotifyState_EmpoweredAura.generated.h"

/**
 * 
 */


UCLASS()
class SOULSGAME_API USAnimNotifyState_EmpoweredAura : public UMyAnimNotifyState
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
