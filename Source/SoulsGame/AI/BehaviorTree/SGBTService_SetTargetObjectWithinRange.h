// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "SGBTService_SetTargetObjectWithinRange.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGBTService_SetTargetObjectWithinRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	USGBTService_SetTargetObjectWithinRange();

	protected:
	virtual void TickNode(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	float TargetRadius = 20000;


	UPROPERTY(EditAnywhere)
	bool bSelectPlayersOnly = true;


	UPROPERTY(EditAnywhere)
	bool Debug_JustSelectPlayer = false;
	
};
