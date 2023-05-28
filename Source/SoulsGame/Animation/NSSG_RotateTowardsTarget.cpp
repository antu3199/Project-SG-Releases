#include "NSSG_RotateTowardsTarget.h"

#include "Kismet/KismetMathLibrary.h"
#include "SoulsGame/SUtils.h"

void UNSSG_RotateTowardsTarget::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);


}

void UNSSG_RotateTowardsTarget::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);
}

void UNSSG_RotateTowardsTarget::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	ASCharacterBase* Character = GetCharacter(MeshComp);
	if (Character == nullptr)
	{
		return;
	}

	USceneComponent* Target = Character->GetFocusTarget();
	if (Target != nullptr)
	{
		FSUtils::RotateCharacterToLookTarget(Character, Target->GetComponentLocation(), bLockZAxis);
	}
}
