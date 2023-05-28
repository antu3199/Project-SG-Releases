#include "NS_RotateActor.h"

#include "Curves/CurveVector.h"

UNS_RotateActor::UNS_RotateActor()
{
}

void UNS_RotateActor::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);


	AActor* Actor = MeshComp->GetAttachmentRootActor();
	if (Actor == nullptr)
	{
		return;
	}

	
	CachedActor = Actor;

	if (IsPreviewingInEditor())
	{
		InitialRotation = FQuat::MakeFromEuler(DefaultPreviewRotation);
	}
	else
	{
		InitialRotation = FQuat(Actor->GetActorRotation());
	}
	
	
}

void UNS_RotateActor::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);
}

void UNS_RotateActor::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance null"));
		return;
	}

	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		UE_LOG(LogTemp, Warning, TEXT("NS_RotateActor: RootMotion null"));
		return;
	}

	AActor* Actor = MeshComp->GetAttachmentRootActor();
	if (Actor == nullptr)
	{
		return;
	}

	if (RotationCurve == nullptr)
	{
		return;
	}
	
	if (IsPreviewingInEditor())
	{
		InitialRotation = FQuat::MakeFromEuler(DefaultPreviewRotation);
	}
	
	const FAnimNotifyEvent* Notify = AnimNotifyEventReference.GetNotify();
	float OffsetNotifyStart = Notify->GetTriggerTime();
	float CurrentPosition = RootMotion->GetPosition();
	float Duration = Notify->GetDuration();
	float Diff = CurrentPosition - OffsetNotifyStart;

	FVector CurValue = RotationCurve->GetVectorValue(Diff);
	FQuat CurRotOffset = FQuat::MakeFromEuler(CurValue);

	FQuat NewRotation = CurRotOffset * InitialRotation;
	Actor->SetActorRotation(NewRotation);
	//FQuat NewRotation =  InitialRotation 
	//Actor->SetActorRotation()
	
}
