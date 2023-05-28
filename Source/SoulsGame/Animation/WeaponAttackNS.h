// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyAnimNotifyState.h"
#include "WeaponAttackNS.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API UWeaponAttackNS : public UMyAnimNotifyState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	bool bVisualsOnly = false;
	
    virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
