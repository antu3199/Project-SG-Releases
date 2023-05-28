#include "NSSG_LockVelocityIfInAir.h"
#include "SoulsGame/UE4Overrides/MyCharacterMovementComponent.h"

void UNSSG_LockVelocityIfInAir::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);
	ASCharacterBase* Character = Cast<ASCharacterBase>(MeshComp->GetAttachmentRootActor());
	if (Character == nullptr)
	{
		return;
	}

	if (UMyCharacterMovementComponent* MovementComponent = Character->GetCharacterMovementComponent())
	{
		if (MovementComponent->IsFalling())
		{
			bAppliedVelocity = true;
			MovementComponent->Velocity.Z = 0.0f;
			MovementComponent->GravityScale = 0.0f;

			if (bModifyRootMotionTranslationScale)
			{
				CachedTranslationScale = Character->GetAnimRootMotionTranslationScale();
				Character->SetAnimRootMotionTranslationScale(0);
			}
		}
	}
	
	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance null"));
		return;
	}

	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		UE_LOG(LogTemp, Warning, TEXT("NS_SetMontageSpeed: RootMotion null"));
		return;
	}
}

void UNSSG_LockVelocityIfInAir::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	ASCharacterBase* Character = Cast<ASCharacterBase>(MeshComp->GetAttachmentRootActor());
	if (Character == nullptr)
	{
		return;
	}

	if (UMyCharacterMovementComponent* MovementComponent = Character->GetCharacterMovementComponent())
	{
		if (bAppliedVelocity)
		{
			bAppliedVelocity = false;
			MovementComponent->ResetGravityScale();

			if (bModifyRootMotionTranslationScale)
			{
				Character->SetAnimRootMotionTranslationScale(CachedTranslationScale);
			}
			
			//if (MovementComponent->IsFalling())
			//{
			//	//MovementComponent->Velocity.Z = 0.0f;
			//}
		}
	}
	
}
