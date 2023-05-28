#include "NS_MoveActor.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SoulsGame/UE4Overrides/MyCharacterMovementComponent.h"

UNS_MoveActor::UNS_MoveActor()
{
}

void UNS_MoveActor::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	AActor* Actor = MeshComp->GetAttachmentRootActor();
	if (Actor == nullptr)
	{
		return;
	}

	
	OriginLocation = Actor->GetActorLocation();
	CachedActor = Actor;
	CachedMeshComp = MeshComp;
	CachedRelativeDirection = GetRelativeDirection(Actor);

	if (IsPreviewingInEditor())
	{
		OriginLocation = FVector::ZeroVector;
	}
	else
	{
		ASCharacterBase* Character = Cast<ASCharacterBase>(Actor);
		if (Character != nullptr)
		{
			UMyCharacterMovementComponent* MovementComponent = Character->GetCharacterMovementComponent();
			DefaultCharacterMovementSpeed = MovementComponent->MaxWalkSpeed;
			if (bChangeMovementMode)
			{
				MovementComponent->SetMovementMode(MOVE_Flying);
			}
			MovementComponent->OverrideNotifies.Add(this);
		}
	}

	if (bChangeCollisionProfileType)
	{
		// TODO: Change profile instead of disabling
		//Actor->SetActorEnableCollision(false);
		// Work with characters only for now to make it easier
		if (ASCharacterBase* Character = Cast<ASCharacterBase>(Actor))
		{
			UCapsuleComponent* Collision = Character->GetCapsuleComponent();
			CachedCollisionProfileName = Collision->GetCollisionProfileName();
			Collision->SetCollisionProfileName(FSGCollisionProfileHelper::GetCollisionProfileName(CollisionProfileType));
		}
	}

	//FVector Rot = Actor->GetActorRotation().Euler();
	//int a = 0;
	
}

void UNS_MoveActor::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	AActor* Actor = MeshComp->GetAttachmentRootActor();
	if (Actor == nullptr)
	{
		return;
	}

	if (IsPreviewingInEditor())
	{
		
		//Actor->SetActorLocation(OriginLocation + GetRelativeDirection(Actor) * Distance);
	}
	else
	{
		ASCharacterBase* Character = Cast<ASCharacterBase>(Actor);

		if (Character != nullptr)
		{

			UMyCharacterMovementComponent* MovementComponent = Character->GetCharacterMovementComponent();

			if (MovementComponent->OverrideNotifies.Contains(this))
			{
				MovementComponent->OverrideNotifies.Remove(this);
			}

			if (bChangeMovementMode)
			{
				MovementComponent->SetDefaultMovementMode();
			}
			
			//MovementComponent->SetMovementMode(MOVE_Walking);
			//MovementComponent->MaxWalkSpeed = DefaultCharacterMovementSpeed;
		}
	}

	if (bChangeCollisionProfileType)
	{
		// TODO: Change profile instead of disabling
		//Actor->SetActorEnableCollision(true);
		if (ASCharacterBase* Character = Cast<ASCharacterBase>(Actor))
		{
			UCapsuleComponent* Collision = Character->GetCapsuleComponent();
			Collision->SetCollisionProfileName(CachedCollisionProfileName);
		}
	}


}

void UNS_MoveActor::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
                               const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);


	
	if (DisableMe == true)
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
		UE_LOG(LogTemp, Warning, TEXT("NS_MoveActor: RootMotion null"));
		return;
	}

	AActor* Actor = MeshComp->GetAttachmentRootActor();
	if (Actor == nullptr)
	{
		return;
	}

	if (bRecalculateDirectionOnTick)
	{
		CachedRelativeDirection = GetRelativeDirection(Actor);
	}

	
	// Note: Origin location is equal to FVector::ZeroVector

	FVector Velocity = GetCurrentVelocity();

	if (IsPreviewingInEditor())
	{
		//CachedRelativeDirection = GetRelativeDirection(Actor);
		// Note: Multiply by 0.01 because velocity is in centimeters
		//Speed *= FrameDeltaTime;
		//FVector MoveAmount =  CachedRelativeDirection * Speed * 0.01f;
		Actor->SetActorLocation(Actor->GetActorLocation() + Velocity);
	}
	else
	{
		ASCharacterBase* Character = Cast<ASCharacterBase>(Actor);
		if (Character == nullptr)
		{
			return;
		}

		// UMyCharacterMovementComponent* MovementComponent = Character->GetCharacterMovementComponent();
		// MovementComponent->Velocity += Velocity;

	}



	//int bone_index = MeshComp->GetBoneIndex( "Root" );
	//FTransform transform;

	//FQuat Rot = MeshComp->GetBoneQuaternion("Root");
	//UE_LOG(LogTemp, Warning, TEXT("Test: %s"), *Rot.ToString());
	//Cast< UAnimSequence >( AnimInstance->SlotAnimTracks 0 ].AnimTrack.AnimSegments 0 ].AnimReference )->GetBoneTransform( transform, bone_index, 0.0f, true );
}

FVector UNS_MoveActor::GetCurrentVelocity() const
{
	if (CachedMeshComp == nullptr)
	{
		return FVector::ZeroVector;
	}
	
	UAnimInstance* AnimInstance = CachedMeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance null"));
		return FVector::ZeroVector;
	}

	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		UE_LOG(LogTemp, Warning, TEXT("NS_MoveActor: RootMotion null"));
		return FVector::ZeroVector;
	}


	
	const FAnimNotifyEvent* Notify = AnimNotifyEventReference.GetNotify();
	float OffsetNotifyStart = Notify->GetTriggerTime();
	float CurrentPosition = RootMotion->GetPosition();
	float Duration = Notify->GetDuration();
	float Diff = CurrentPosition - OffsetNotifyStart;
	float t = Diff / Duration;
	
	float Speed;
	if (MovementSpeedCurve != nullptr)
	{
		float CurveValue = MovementSpeedCurve->GetFloatValue(Diff);
		Speed = CurveValue * MovementSpeedScaleFactor;
	}
	else
	{
		Speed = MovementSpeedScaleFactor;
	}

	FVector OGVelocity;

	if (ASCharacterBase* Character = Cast<ASCharacterBase>(CachedActor))
	{
		if (UMyCharacterMovementComponent* MovementComponent = Character->GetCharacterMovementComponent())
		{
			OGVelocity = MovementComponent->Velocity;
		}
	}
	
	
	FVector FinalVelocity;
	if (IsPreviewingInEditor())
	{
		FinalVelocity = CachedRelativeDirection * Speed * CachedMeshComp->GetWorld()->GetDeltaSeconds();
	}
	else
	{
		FinalVelocity = CachedRelativeDirection * Speed;
	}

	if (bUnlockX)
	{
		FinalVelocity.X = OGVelocity.X;
	}

	if (bUnlockY)
	{
		FinalVelocity.Y = OGVelocity.Y;
	}

	if (bUnlockZ)
	{
		FinalVelocity.Z = OGVelocity.Z;
	}

	return FinalVelocity;
}

FVector UNS_MoveActor::GetRelativeDirection(AActor* Actor) const
{
	if (IsPreviewingInEditor())
	{
		return Direction;
	}
	
	FTransform Transform = Actor->GetTransform();
	FVector RealDirection = Transform.TransformVector(Direction);
	return RealDirection;
}

