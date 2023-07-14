#include "SGAnimNotifyState_StopMontageOnForward.h"

void USGAnimNotifyState_StopMontageOnForward::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                            float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);
}

void USGAnimNotifyState_StopMontageOnForward::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);
}

void USGAnimNotifyState_StopMontageOnForward::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (DisableMe)
	{
		return;
	}
	
	AActor* PawnCharacter = MeshComp->GetAttachmentRootActor();
	if (PawnCharacter == nullptr)
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
	
	// Line trace
	FVector StartLocation = PawnCharacter->GetActorLocation();
	FVector EndLocation = StartLocation + PawnCharacter->GetActorForwardVector() * ForwardDistance;
    
	FHitResult HitResult;
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(PawnCharacter); // Ignore the actor doing the line trace
	Params.bTraceComplex = true; // Trace against complex collision shapes
	Params.bReturnPhysicalMaterial = false; // Do not return physical material information

	TArray<struct FHitResult> OutHits;
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	const bool bHit = MeshComp->GetWorld()->LineTraceMultiByObjectType(OutHits, StartLocation, EndLocation, ObjectParams, Params);
	if (bHit)
	{
		// The line trace hit an actor in the "WorldStatic" or "Pawn" collision channel
		// Handle the collision however you want
		//for (const FHitResult& Hit : OutHits)
		//{
		//	UE_LOG(LogTemp, Display, TEXT("HIT: %s"), *Hit.GetActor()->GetActorLabel());
		//}
		FMontageBlendSettings Settings;
		RootMotion->Stop(Settings);
	}
	else
	{
		// The line trace did not hit anything in the "WorldStatic" or "Pawn" collision channel
		// Root motion is not being blocked by a wall or pawn
	}
	
}

