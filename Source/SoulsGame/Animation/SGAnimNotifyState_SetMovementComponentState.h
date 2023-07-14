// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_SetMovementComponentState.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGAnimNotifyState_SetMovementComponentState : public USGAnimNotifyState
{
	GENERATED_BODY()
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;


	UPROPERTY(EditAnywhere)
	bool bDoMovementModeStart = true;
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EMovementMode> MovementModeStart;

	UPROPERTY(EditAnywhere)
	bool bDoMovementModeEnd = false;
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EMovementMode> MovementModeEnd;
	
};
