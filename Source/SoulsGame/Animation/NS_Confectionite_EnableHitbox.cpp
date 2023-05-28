// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_Confectionite_EnableHitbox.h"

void UNS_Confectionite_EnableHitbox::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);
}

void UNS_Confectionite_EnableHitbox::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);
}
