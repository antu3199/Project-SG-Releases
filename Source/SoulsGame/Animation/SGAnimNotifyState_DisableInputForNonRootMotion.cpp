#include "SGAnimNotifyState_DisableInputForNonRootMotion.h"

void USGAnimNotifyState_DisableInputForNonRootMotion::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                                    float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	ASGCharacterBase * Character = GetCharacter(MeshComp);
	if (Character == nullptr)
	{
		return;
	}

	Character->SetDisableInputForNonRootMotion(DisableInputForNonRootMotionStart);
	
}

void USGAnimNotifyState_DisableInputForNonRootMotion::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	ASGCharacterBase * Character = GetCharacter(MeshComp);
	if (Character == nullptr)
	{
		return;
	}

	Character->SetDisableInputForNonRootMotion(DisableInputForNonRootMotionEnd);
}
