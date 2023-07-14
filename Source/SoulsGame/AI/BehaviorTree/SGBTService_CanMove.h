// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "SGBTService_CanMove.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGBTService_CanMove: public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	USGBTService_CanMove();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
