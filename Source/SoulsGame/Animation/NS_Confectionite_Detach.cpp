// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_Confectionite_Detach.h"

void UNS_Confectionite_Detach::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	AConfectioniteCharacter * Character = Cast<AConfectioniteCharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}

	//Character->SetDebugMode(true);
}
