// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyCharacterMovementComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnFreezeFrameConditionMet);


/**
 * 
 */
UCLASS()
class SOULSGAME_API UMyCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()


public:
	virtual void BeginPlay() override;
	
	UPROPERTY(Transient)
	TSet<class UNS_MoveActor*> OverrideNotifies;

	void AddFreezeFrameCondition(const TFunction<bool()>& Condition);

	FOnFreezeFrameConditionMet OnFreezeFrameConditionMet;
	
	bool StopRootMotionMovement = false;

	bool HasPendingFreezeConditionMet() const;
	void ConsumeFreezeCondition();

	virtual bool DoJump(bool bReplayingMoves) override;

	void ResetGravityScale();
	
private:
	UPROPERTY(EditAnywhere, Category="Custom")
	bool OverrideVelocityWithRootMotion = true;
	
	virtual FVector ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const override;
	
	virtual void SimulateMovement(float DeltaSeconds) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void PerformMovement(float DeltaSeconds) override;
	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;


	int JumpNumber = 0;
	int FreezeConditionsMet = 0;
	TArray<TFunction<bool()>> FreezeFrameConditions;

	float DefaultGravityScale = 1.0f;
	
};
