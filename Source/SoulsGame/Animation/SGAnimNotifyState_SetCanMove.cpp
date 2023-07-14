// Fill out your copyright notice in the Description page of Project Settings.


#include "SGAnimNotifyState_SetCanMove.h"

void USGAnimNotifyState_SetCanMove::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);
	
	ASGCharacterBase * Character = GetCharacter(MeshComp);

	if (Character == nullptr)
	{
		return;
	}

	Character->SetCanMove(CanMoveNotifyStart);
}

void USGAnimNotifyState_SetCanMove::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);
	
	ASGCharacterBase * Character = GetCharacter(MeshComp);

	if (Character == nullptr)
	{
		return;
	}

	Character->SetCanMove(CanMoveNotifyEnd);
}
