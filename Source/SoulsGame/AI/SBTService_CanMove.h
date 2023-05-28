// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "SBTService_CanMove.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USBTService_CanMove: public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	USBTService_CanMove();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
