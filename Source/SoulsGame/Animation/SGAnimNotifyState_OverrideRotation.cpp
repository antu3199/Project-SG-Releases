// Fill out your copyright notice in the Description page of Project Settings.


#include "SGAnimNotifyState_OverrideRotation.h"

void USGAnimNotifyState_OverrideRotation::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                        float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	ASGHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
	if (!Character)
	{
		return;
	}

	Character->SetOverrideRotation(true);
}

void USGAnimNotifyState_OverrideRotation::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	ASGHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
	if (!Character)
	{
		return;
	}

	Character->SetOverrideRotation(false);
}
