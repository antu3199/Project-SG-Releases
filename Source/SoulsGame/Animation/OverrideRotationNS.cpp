// Fill out your copyright notice in the Description page of Project Settings.


#include "OverrideRotationNS.h"

void UOverrideRotationNS::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	ASHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
	if (!Character)
	{
		return;
	}

	Character->SetOverrideRotation(true);
}

void UOverrideRotationNS::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	ASHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
	if (!Character)
	{
		return;
	}

	Character->SetOverrideRotation(false);
}
