#include "SGAnimNotifyState_LoopAnim.h"

void USGAnimNotifyState_LoopAnim::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance null"));
		return;
	}

	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		UE_LOG(LogTemp, Warning, TEXT("NS_LoopAnim: RootMotion null"));
		return;
	}

	
}

void USGAnimNotifyState_LoopAnim::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance null"));
		return;
	}

	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		UE_LOG(LogTemp, Warning, TEXT("NS_LoopAnim: RootMotion null"));
		return;
	}


	bool bRepeatLoop = true;
	if (NumLoops != -1)
	{
		CurNumLoops++;

		if (CurNumLoops >= NumLoops)
		{
			CurNumLoops = 0;
			bRepeatLoop = false;
		}
		
	}


	if (bRepeatLoop)
	{
		const FAnimNotifyEvent* Notify = AnimNotifyEventReference.GetNotify();
		float OffsetNotifyStart = Notify->GetTriggerTime();

  		RootMotion->SetPosition(OffsetNotifyStart);
	}



	
}
