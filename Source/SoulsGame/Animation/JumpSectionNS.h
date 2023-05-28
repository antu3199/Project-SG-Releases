// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAnimNotifyState.h"
#include "JumpSectionNS.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API UJumpSectionNS : public UMyAnimNotifyState
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FName> NextMontageNames;

	UPROPERTY(EditAnywhere)
	bool bAllowInputOnNotifyEnd = false;

	UPROPERTY(EditAnywhere)
	bool CancelMontageOnAllowInput = true;
	
	
protected:
	virtual void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;
};
