#include "SGAnimNotifyState_ResetPreviewActor.h"

#include "GameFramework/CharacterMovementComponent.h"

USGAnimNotifyState_ResetPreviewActor::USGAnimNotifyState_ResetPreviewActor()
{
}

void USGAnimNotifyState_ResetPreviewActor::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	bAbleToTrigger = true;
}

void USGAnimNotifyState_ResetPreviewActor::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!bAbleToTrigger)
	{
		return;
	}

	bAbleToTrigger = false;	
	AActor* Actor = MeshComp->GetAttachmentRootActor();
	if (Actor == nullptr)
	{
		return;
	}
	
	if (IsPreviewingInEditor())
	{
		Actor->SetActorLocation(FVector::ZeroVector);
		MeshComp->SetRelativeRotation(FQuat::MakeFromEuler(InitialRotation));
	}
	else
	{
		if (bResetInGameVelocityAsWell)
		{
			ASGCharacterBase* Character = Cast<ASGCharacterBase>(Actor);
			if (Character == nullptr)
			{
				return;
			}

			UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();
			MovementComponent->Velocity = FVector::ZeroVector;
		}
	}
}
