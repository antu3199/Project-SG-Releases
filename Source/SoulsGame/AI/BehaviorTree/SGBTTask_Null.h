// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SGBTTask_Null.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USGBTTask_Null : public UBTTaskNode
{
	GENERATED_BODY()
public:
	USGBTTask_Null();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
