// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_SetCanMove.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGAnimNotifyState_SetCanMove : public USGAnimNotifyState
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool CanMoveNotifyStart = false;

	UPROPERTY(EditAnywhere)
	bool CanMoveNotifyEnd = true;


protected:
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

};
