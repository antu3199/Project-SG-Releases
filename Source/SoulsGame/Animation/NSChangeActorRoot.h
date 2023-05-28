// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAnimNotifyState.h"
#include "NSChangeActorRoot.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API UNSChangeActorRoot : public UMyAnimNotifyState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	FString SocketName;

	UPROPERTY(EditAnywhere)
	bool SnapToTarget;
	
	void DoNotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;
	
};
