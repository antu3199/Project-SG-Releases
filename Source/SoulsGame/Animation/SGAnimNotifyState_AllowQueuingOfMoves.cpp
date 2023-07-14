#include "SGAnimNotifyState_AllowQueuingOfMoves.h"

#include "SoulsGame/Character/SGPlayerController.h"

void USGAnimNotifyState_AllowQueuingOfMoves::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                           float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	ASGHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
	if (!Character)
	{
		return;
	}
	
	// Need to turn on buffered input
	if (ASGPlayerController* Controller = Cast<ASGPlayerController>(Character->GetController()))
	{
		Controller->SetCanQueueNextMove(true);
	}
}

void USGAnimNotifyState_AllowQueuingOfMoves::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);
}
