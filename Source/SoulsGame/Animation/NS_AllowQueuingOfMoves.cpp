#include "NS_AllowQueuingOfMoves.h"

#include "SoulsGame/Character/MyPlayerController.h"

void UNS_AllowQueuingOfMoves::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                            float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	ASHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
	if (!Character)
	{
		return;
	}
	
	// Need to turn on buffered input
	if (AMyPlayerController* Controller = Cast<AMyPlayerController>(Character->GetController()))
	{
		Controller->SetCanQueueNextMove(true);
	}
}

void UNS_AllowQueuingOfMoves::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);
}
