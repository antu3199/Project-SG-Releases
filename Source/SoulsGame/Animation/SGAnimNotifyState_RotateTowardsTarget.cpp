#include "SGAnimNotifyState_RotateTowardsTarget.h"

#include "Kismet/KismetMathLibrary.h"
#include "SoulsGame/SGUtils.h"

void USGAnimNotifyState_RotateTowardsTarget::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                           float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);


}

void USGAnimNotifyState_RotateTowardsTarget::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);
}

void USGAnimNotifyState_RotateTowardsTarget::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	ASGCharacterBase* Character = GetCharacter(MeshComp);
	if (Character == nullptr)
	{
		return;
	}

	USceneComponent* Target = Character->GetFocusTarget();
	if (Target != nullptr)
	{
		FSGUtils::RotateCharacterToLookTarget(Character, Target->GetComponentLocation(), bLockZAxis);
	}
}
