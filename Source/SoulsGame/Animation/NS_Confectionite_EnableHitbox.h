// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAnimNotifyState.h"
#include "NS_Confectionite_EnableHitbox.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API UNS_Confectionite_EnableHitbox : public UMyAnimNotifyState
{
	GENERATED_BODY()
	
protected:

	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
