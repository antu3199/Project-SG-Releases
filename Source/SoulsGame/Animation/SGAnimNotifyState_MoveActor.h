// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SGAnimNotifyState.h"
#include "SGAnimNotifyState_MoveActor.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGAnimNotifyState_MoveActor : public USGAnimNotifyState
{
	GENERATED_BODY()

	USGAnimNotifyState_MoveActor();

public:
	FVector GetCurrentVelocity() const;

protected:
	virtual void DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void DoNotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override; 

	
	UPROPERTY(EditAnywhere)
	FVector Direction = FVector::OneVector;

	UPROPERTY(EditAnywhere)
	UCurveFloat* MovementSpeedCurve;

	UPROPERTY(EditAnywhere)
	bool bRecalculateDirectionOnTick = false;
	
	FVector OriginLocation = FVector::ZeroVector;

	UPROPERTY(Transient)
	AActor* CachedActor = nullptr;

	UPROPERTY(Transient)
	USkeletalMeshComponent* CachedMeshComp = nullptr;
	
	UPROPERTY(EditAnywhere)
	float MovementSpeedScaleFactor = 100000;

	UPROPERTY(EditAnywhere)
	bool bChangeCollisionProfileType = false;

	UPROPERTY(EditAnywhere)
	bool bChangeMovementMode = true;

	UPROPERTY(EditAnywhere)
	ESGCollisionProfileType CollisionProfileType;

	UPROPERTY(EditAnywhere)
	bool bUnlockX = false;

	UPROPERTY(EditAnywhere)
	bool bUnlockY = false;

	UPROPERTY(EditAnywhere)
	bool bUnlockZ = false;

	FVector GetRelativeDirection(AActor* Actor) const;

	FVector CachedRelativeDirection;

	FName CachedCollisionProfileName;

	float DefaultCharacterMovementSpeed = 1;
};
