// Fill out your copyright notice in the Description page of Project Settings.


#include "NSConvertToWorldSpaceRotation.h"


#include "GeometryCollection/GeometryCollectionAlgo.h"
#include "Kismet/KismetMathLibrary.h"

void UNSConvertToWorldSpaceRotation::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                   float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	AActor * Actor = MeshComp->GetOwner();
	if (!Actor)
	{
		return;
	}

	AActor * ParentActor = Actor->GetOwner();
	if (!ParentActor)
	{
		return;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	UAnimMontage * CurrentActiveMontage = AnimInstance->GetCurrentActiveMontage();
	if (!CurrentActiveMontage)
	{
		return;
	}

	// Cache activemontage to do the final rotation in the final frame
	ActiveMontage = CurrentActiveMontage;

	float CurrentTime = AnimInstance->Montage_GetPosition(CurrentActiveMontage);
	PreviousTransform = GetBoneTransFromMontage(CurrentActiveMontage, FName("controller"), CurrentTime);

	InitialParentTransform = ParentActor->GetTransform();

	InitialActorLocation = Actor->GetActorLocation();
	InitialBoneLocation = PreviousTransform.GetLocation();
	InitialRelativeRotation = Actor->GetRootComponent()->GetRelativeRotation();
	
}

void UNSConvertToWorldSpaceRotation::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	AActor * Actor = MeshComp->GetOwner();
	if (!Actor)
	{
		return;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	if (!ActiveMontage)
	{
		return;
	}

	float LastFrameTime = ActiveMontage->GetSectionLength(0);
	DoRotation(MeshComp, LastFrameTime);
}

void UNSConvertToWorldSpaceRotation::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (!ActiveMontage)
		return;


	DoRotation(MeshComp);

}


FTransform UNSConvertToWorldSpaceRotation::GetBoneTransFromMontage(UAnimMontage* InMontage, FName BoneName, float InTime)
{    
	FTransform Transform = FTransform();
	if (InMontage)
	{
		int bone_index = 0;

		USkeletalMesh* SkelMesh = InMontage->GetSkeleton()->GetPreviewMesh(true);
		if (SkelMesh)
		{
			bone_index = SkelMesh->GetRefSkeleton().FindBoneIndex(BoneName);
			UAnimSequence* Anim = Cast<UAnimSequence>(InMontage->SlotAnimTracks[0].AnimTrack.AnimSegments[0].AnimReference);

			if (bone_index >= 0)
			{
				Anim->GetBoneTransform(Transform, bone_index, InTime, true);
			}
		}
	}
	return Transform;
}

FQuat UNSConvertToWorldSpaceRotation::GetRotationFromAnimation(const FTransform& Transform) const
{
	return Transform.GetRotation() * FQuat::MakeFromEuler(FVector(90, 0, -180));
}

void UNSConvertToWorldSpaceRotation::DoRotation(USkeletalMeshComponent* MeshComp, float TestTime)
{

	AActor * Actor = MeshComp->GetOwner();
	if (!Actor)
	{
		return;
	}

	UAnimMontage * CurrentActiveMontage = ActiveMontage;
	
	float CurrentTime;
	if (TestTime == -1)
	{
		UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
		if (!AnimInstance)
		{
			return;
		}

		CurrentTime = AnimInstance->Montage_GetPosition(CurrentActiveMontage);
	}
	else
	{
		CurrentTime = TestTime;
	}


	if( FMath::IsNearlyEqual(CurrentTime, 0) || CurrentTime < 0)
	{
		return;
	}
	
	FTransform CurTransform = GetBoneTransFromMontage(CurrentActiveMontage, FName("controller"), CurrentTime);

	// Note: Working in absolutes disables blending, but is more accurate.
	if (!AdditiveAnimation)
	{
		FVector DeltaLocation = (CurTransform.GetLocation() - InitialBoneLocation);
		DeltaLocation = FQuat::MakeFromEuler(FVector(90, 0, 0)) * DeltaLocation;
		DeltaLocation = FQuat::MakeFromEuler(FVector(0, 0, -90)) * InitialParentTransform.GetRotation()  * DeltaLocation;
		DeltaLocation *= 100;

		FVector CurLocation = InitialActorLocation +  DeltaLocation;

	
		Actor->SetActorLocation(CurLocation, true);
	

		FQuat CurRotationQuat = CurTransform.GetRotation();
		FVector CurRotationEuler = CurRotationQuat.Euler();
		CurRotationEuler = FVector(CurRotationEuler.X + 90, -CurRotationEuler.Y, -CurRotationEuler.Z + 180 );

	
		FRotator Rot(  InitialRelativeRotation.Quaternion() * FQuat::MakeFromEuler(CurRotationEuler) );

		FHitResult Result;
		Actor->SetActorRelativeRotation(Rot  , true, nullptr, ETeleportType::None);

	}
	else
	{
		FVector DeltaLocation = CurTransform.GetLocation() - PreviousTransform.GetLocation();
		DeltaLocation = FQuat::MakeFromEuler(FVector(90, 0, 0)) * DeltaLocation;
		DeltaLocation = FQuat::MakeFromEuler(FVector(0, 0, -90)) * InitialParentTransform.GetRotation()  * DeltaLocation;
		DeltaLocation *= 100;

		Actor->SetActorLocation(Actor->GetTransform().GetLocation() + DeltaLocation, true);

		FVector InverseEuler = PreviousTransform.GetRotation().Euler();
		InverseEuler = FVector(InverseEuler.X + 90, -InverseEuler.Y, -InverseEuler.Z + 180 );

		FVector CurEuler = CurTransform.GetRotation().Euler();
		CurEuler = FVector(CurEuler.X + 90, -CurEuler.Y, -CurEuler.Z + 180 );
		
		FQuat DeltaQuat =  FQuat::MakeFromEuler(InverseEuler).Inverse() * FQuat::MakeFromEuler(CurEuler);
		FVector DeltaQuatEuler = DeltaQuat.Euler();
		
		FRotator Rot(  FQuat::MakeFromEuler(DeltaQuatEuler) );
		Actor->AddActorLocalRotation(Rot, true);
	}

	PreviousTransform = CurTransform;
}
