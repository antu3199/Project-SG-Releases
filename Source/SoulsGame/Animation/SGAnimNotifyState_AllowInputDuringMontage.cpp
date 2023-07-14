// Fill out your copyright notice in the Description page of Project Settings.


#include "SGAnimNotifyState_AllowInputDuringMontage.h"

#include "SoulsGame/Character/SGPlayerController.h"

void USGAnimNotifyState_AllowInputDuringMontage::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                               float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	ASGHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
	if (!Character)
	{
		return;
	}


	Character->SetAllowInputDuringMontage(true);
	Character->SetMontageCancellable(CancelMontage);
}

void USGAnimNotifyState_AllowInputDuringMontage::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	ASGHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
	if (!Character)
	{
		return;
	}

	Character->SetAllowInputDuringMontage(false);
	Character->SetMontageCancellable(false);

}
