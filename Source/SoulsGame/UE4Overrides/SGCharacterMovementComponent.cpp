// Fill out your copyright notice in the Description page of Project Settings.


#include "SGCharacterMovementComponent.h"

#include "SoulsGame/Animation/SGAnimNotifyState_MoveActor.h"

void USGCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	DefaultGravityScale = GravityScale;
}

void USGCharacterMovementComponent::AddFreezeFrameCondition(const TFunction<bool()>& Condition)
{
	FreezeFrameConditions.Add(Condition);
}

bool USGCharacterMovementComponent::HasPendingFreezeConditionMet() const
{
	return FreezeConditionsMet > 0;
}

void USGCharacterMovementComponent::ConsumeFreezeCondition()
{
	FreezeConditionsMet--;
}

bool USGCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	// Default implementation
	if ( CharacterOwner && CharacterOwner->CanJump() )
	{
		// Don't jump if we can't move up/down.
		if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.f)
		{
			Velocity.Z = FMath::Max<FVector::FReal>(Velocity.Z, JumpZVelocity);
			SetMovementMode(MOVE_Falling);
			return true;
		}
	}
	
	return false;
}

void USGCharacterMovementComponent::ResetGravityScale()
{
	GravityScale = DefaultGravityScale;
}


FVector USGCharacterMovementComponent::ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity,
                                                                       const FVector& CurrentVelocity) const
{

	return Super::ConstrainAnimRootMotionVelocity(RootMotionVelocity, CurrentVelocity);
	if (StopRootMotionMovement)
	{
		return FVector::ZeroVector;
	}
	
	FVector Result = RootMotionVelocity;

	if (OverrideVelocityWithRootMotion)
	{
		// Do not override Velocity.Z if in falling physics, we want to keep the effect of gravity.
		if (IsFalling())
		{
			Result.Z = CurrentVelocity.Z;
		}
	}
	else
	{
		Result += CurrentVelocity;
		//Result = CurrentVelocity;
	}

	return Result;
}


void USGCharacterMovementComponent::SimulateMovement(float DeltaSeconds)
{
	Super::SimulateMovement(DeltaSeconds);
}

void USGCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OverrideNotifies.IsEmpty())
	{
		FVector FinalVelocity;

		for (auto& Notify : OverrideNotifies)
		{
			if (Notify == nullptr)
			{
				continue;
			}
			
			FinalVelocity += Notify->GetCurrentVelocity();
		}

		Velocity = FinalVelocity;
	}

	if (!FreezeFrameConditions.IsEmpty())
	{
		TArray<int32> IndicesToRemove;
		for (int32 i = FreezeFrameConditions.Num() - 1; i >= 0; i--)
		{
			if (FreezeFrameConditions[i] != nullptr && FreezeFrameConditions[i]())
			{
				if (OnFreezeFrameConditionMet.IsBound())
				{
					OnFreezeFrameConditionMet.Broadcast();
				}
				else
				{
					FreezeConditionsMet++;
				}
				IndicesToRemove.Add(i);
			}
		}
		
		for (auto & IndexToRemove : IndicesToRemove)
		{
			FreezeFrameConditions.RemoveAt(IndexToRemove);
		}
	}
}

void USGCharacterMovementComponent::PerformMovement(float DeltaSeconds)
{
	// Sometimes the AI messes up movement for some reason. Bandage solution.
	if (Velocity.ContainsNaN())
	{
		return;
	}
	
	Super::PerformMovement(DeltaSeconds);
}

void USGCharacterMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid,
	float BrakingDeceleration)
{
	Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
}

