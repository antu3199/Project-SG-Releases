// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTask_SetScoot.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USBTTask_SetScoot : public UBTTaskNode
{
	GENERATED_BODY()
public:
	USBTTask_SetScoot();

	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector DoScootBlackboardKey;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector ScootLocationBlackboardKey;

	UPROPERTY(EditAnywhere)
	float ChanceToScoot = 1.0;

	UPROPERTY(EditAnywhere)
	float MinScootDistance = 250;
	
	UPROPERTY(EditAnywhere)
	float MaxScootDistance = 250;
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	EBTNodeResult::Type DoScoot(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
};
