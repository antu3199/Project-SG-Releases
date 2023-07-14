// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "SGBTService_IsDead.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGBTService_IsDead : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	USGBTService_IsDead();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
