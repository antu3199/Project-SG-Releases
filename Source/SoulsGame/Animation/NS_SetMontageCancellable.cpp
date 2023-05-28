// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_SetMontageCancellable.h"

void UNS_SetMontageCancellable::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);
	ASHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
	if (!Character)
	{
		return;
	}
	Character->SetMontageCancellable(true);
}

void UNS_SetMontageCancellable::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);
	
	ASHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
	if (!Character)
	{
		return;
	}
	
	Character->SetMontageCancellable(false);
}
