#include "NS_FreezeFrame.h"

#include "SoulsGame/UE4Overrides/MyCharacterMovementComponent.h"

void UNS_FreezeFrame::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	AActor* Actor = MeshComp->GetAttachmentRootActor();
	if (Actor == nullptr)
	{
		return;
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
		UE_LOG(LogTemp, Warning, TEXT("NS_FreezeFrame: RootMotion null"));
		return;
	}

	CachedActor = Actor;
	CachedMeshComp = MeshComp;
	
	float MontageSpeed = FrozenMontageSpeed;

	if (ASCharacterBase* CharacterBase = Cast<ASCharacterBase>(GetCharacter(MeshComp)))
	{
		BasePlayRate = CharacterBase->GetCurrentMontageSpeed();
		CharacterBase->SetAllowSetMontageSpeed(false);

		if (bListenToFreezeFrameCondition)
		{
			if (UMyCharacterMovementComponent* MovementComponent = CharacterBase->GetCharacterMovementComponent())
			{
				if (!MovementComponent->HasPendingFreezeConditionMet())
				{
					MovementComponent->OnFreezeFrameConditionMet.AddUObject(this, &UNS_FreezeFrame::OnFreezeFrameConditionMet);
				}
				else
				{
					// Condition was already true!
					OnFreezeFrameConditionMet();
					MovementComponent->ConsumeFreezeCondition();
				}
			}
		}
	}
	
	RootMotion->SetPlayRate(MontageSpeed);
	bActivated = true;

	if (FreezeFrameTime > 0)
	{
		
		MeshComp->GetWorld()->GetTimerManager().SetTimer(FreezeTimerHandle, this, &UNS_FreezeFrame::ResetAnim, FreezeFrameTime, false, FreezeFrameTime);
	}

	
}

void UNS_FreezeFrame::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	if (bActivated)
	{
		ResetAnim();
	}
}

void UNS_FreezeFrame::ResetAnim()
{
	UAnimInstance* AnimInstance = CachedMeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance null"));
		return;
	}

	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		UE_LOG(LogTemp, Warning, TEXT("NS_FreezeFrame: RootMotion null"));
		return;
	}

	CachedMeshComp->GetWorld()->GetTimerManager().ClearTimer(FreezeTimerHandle);

	float MontageSpeed = BasePlayRate;

	if (ASCharacterBase* CharacterBase = Cast<ASCharacterBase>(GetCharacter(CachedMeshComp)))
	{
		CharacterBase->SetAllowSetMontageSpeed(true);
		MontageSpeed = CharacterBase->GetFinalMontageSpeed();

		if (bListenToFreezeFrameCondition)
		{
			if (UMyCharacterMovementComponent* MovementComponent = CharacterBase->GetCharacterMovementComponent())
			{
					
				MovementComponent->OnFreezeFrameConditionMet.RemoveAll(this);
			}
		}
		
	}
	
	RootMotion->SetPlayRate(MontageSpeed);

	bActivated = false;
}

void UNS_FreezeFrame::OnFreezeFrameConditionMet()
{
	ResetAnim();
}
