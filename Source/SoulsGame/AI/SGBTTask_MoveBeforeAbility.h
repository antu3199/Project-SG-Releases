// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAIHelpers.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Abilities/GameplayAbilityTypes.h"

#include "SGBTTask_MoveBeforeAbility.generated.h"

struct FMoveBeforeAbilityMemory
{
	bool bAbilityEnded = false;

	TWeakObjectPtr<class ASCharacterBase> CachedCharacter;
	TWeakObjectPtr<AActor> CachedTarget;
	TWeakObjectPtr<AAIController> CachedAIController;

	float SecondsPassed = 0.0f;
	float MoveDuration = 1.0f;

	float AcceptableDistance = 150.0f;
	float PrecisionDot;

	EAIWalkDirection CachedWalkDirection;
	
};


/**
 * 
 */
UCLASS()
class SOULSGAME_API USGBTTask_MoveBeforeAbility : public UBTTaskNode
{
	GENERATED_BODY()
public:
	USGBTTask_MoveBeforeAbility();

	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector AbilityHandleBlackboardKey;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector AbilityHistoryBlackboardKey;
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	void EndTask(uint8* NodeMemory);

	uint16 GetInstanceMemorySize() const override;

	void SwapActionWithQueued(UBehaviorTreeComponent& OwnerComp);

	void LookAtTarget(FMoveBeforeAbilityMemory& MyMemory);

	FVector GetCircularMotionVectorCCW(const float& DeltaTime) const;

	UPROPERTY(EditAnywhere)
	float Debug_MoveDuration = -1.0f;

	UPROPERTY(EditAnywhere)
	float MoveAxisScale = 1.0f;

	
	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector TargetActorBlackboardKey;


};
