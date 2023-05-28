#include "NS_SetMovementComponentState.h"

#include "GameFramework/CharacterMovementComponent.h"

void UNS_SetMovementComponentState::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                  float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	if (!bDoMovementModeStart)
	{
		return;
	}

	if (ACharacter* CharacterBase = Cast<ACharacter>(MeshComp->GetAttachmentRootActor()))
	{
		if (UCharacterMovementComponent* MovementComponent = CharacterBase->GetCharacterMovement())
		{
			MovementComponent->SetMovementMode(MovementModeStart);
		}
	}
}

void UNS_SetMovementComponentState::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	if (!bDoMovementModeEnd)
	{
		return;
	}

	if (ACharacter* CharacterBase = Cast<ACharacter>(MeshComp->GetAttachmentRootActor()))
	{
		if (UCharacterMovementComponent* MovementComponent = CharacterBase->GetCharacterMovement())
		{
			MovementComponent->SetMovementMode(MovementModeEnd);
		}
	}
}
