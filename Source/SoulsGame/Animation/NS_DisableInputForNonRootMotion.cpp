#include "NS_DisableInputForNonRootMotion.h"

void UNS_DisableInputForNonRootMotion::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	ASCharacterBase * Character = GetCharacter(MeshComp);
	if (Character == nullptr)
	{
		return;
	}

	Character->SetDisableInputForNonRootMotion(DisableInputForNonRootMotionStart);
	
}

void UNS_DisableInputForNonRootMotion::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	ASCharacterBase * Character = GetCharacter(MeshComp);
	if (Character == nullptr)
	{
		return;
	}

	Character->SetDisableInputForNonRootMotion(DisableInputForNonRootMotionEnd);
}
