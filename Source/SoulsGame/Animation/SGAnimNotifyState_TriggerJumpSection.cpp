// Fill out your copyright notice in the Description page of Project Settings.


#include "SGAnimNotifyState_TriggerJumpSection.h"

void USGAnimNotifyState_TriggerJumpSection::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                          float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	/*
	ACharacterBase * Character = GetCharacter(MeshComp);

	if (Character == nullptr)
	{
		return;
	}

	Character->JumpSectionCancellable = true;
	Character->TriggerJumpSectionCombo();
	*/
}

void USGAnimNotifyState_TriggerJumpSection::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	/*
	ACharacterBase * Character = GetCharacter(MeshComp);

	if (Character == nullptr)
	{
		return;
	}

	Character->JumpSectionCancellable = false;
	*/
}
