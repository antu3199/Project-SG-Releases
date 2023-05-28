#include "NS_ResetPreviewActor.h"

#include "GameFramework/CharacterMovementComponent.h"

UNS_ResetPreviewActor::UNS_ResetPreviewActor()
{
}

void UNS_ResetPreviewActor::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	bAbleToTrigger = true;
}

void UNS_ResetPreviewActor::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
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
			ASCharacterBase* Character = Cast<ASCharacterBase>(Actor);
			if (Character == nullptr)
			{
				return;
			}

			UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();
			MovementComponent->Velocity = FVector::ZeroVector;
		}
	}
}
